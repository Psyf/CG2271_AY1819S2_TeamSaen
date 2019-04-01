#define __AVR_ATmega328P__

#include <Arduino.h>
#include <avr/io.h>
//#include <String.h>
//#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#define STACK_SIZE 200

#include "audioDriver.h"
#include "ledDriver.h"
#include "motorDriver.h"
#include "bluetoothDriver.h"

//TODO: put in separate file as enum
#define STATIONARY 'S'
#define MOVING 'M'
#define WON 'W'

//TODO: ReEvaluate
#define MESSAGE_LEN 6

// Poster: xTaskBluetooth
// Taker: xTaskMotor
// Single item Queue because we should ensure xTaskMotor is never too late in responding.
// Ideally, make sure item in queue is taken care of before next item comes in (t>MESSAGE_LENms)
QueueHandle_t xMotorCommandQueue = xQueueCreate(1, sizeof(char)*MESSAGE_LEN);

// Poster: xTaskMotor
// Taker: xTaskLED and xTaskAudio respectively
// Single item Queue again because should be responsive enough
QueueHandle_t xLEDStatusQueue = xQueueCreate(1, sizeof(char));
QueueHandle_t xAudioStatusQueue = xQueueCreate(1, sizeof(char));

// Poster: All other Task
// Taker: xTaskDebugger
// Use this queue to use Serial, for Debugging purposes
// !!! Make sure to Null Terminate your char* !!!
QueueHandle_t xDebuggerQueue = xQueueCreate(5, sizeof(char*));

// !!! please Disable if using Hardware Serial0 for other purposes
// TODO: [BUG] Serial won't run for more than 1 task.
void xTaskDebugger(void *p) {
	char* strPointer;
	for(;;) {
		xQueueReceive(xDebuggerQueue, &strPointer, portMAX_DELAY);
		Serial.println(strPointer);
	}
}

//HighestPriority
//TODO: Do by Interrupt();
void xTaskBluetooth(void *p) {
	TickType_t lastWakeTime = xTaskGetTickCount();
	char buf[MESSAGE_LEN];
	unsigned int ptr = 0;
	for (;;) {
		for(int i=0; i<MESSAGE_LEN; i++){
			char c = receiveChar();
			// if Delimiter,
			// since HC-06 doesn't use /r or /n
			if (c != '\0') {
					buf[ptr++] = c;
			}

			if (c == '.' ) {
				if (ptr == MESSAGE_LEN){ //send to Q
					char char_array[MESSAGE_LEN-1]; // stripping the .
					memcpy(char_array,buf, sizeof(char_array));
					if (xQueueSendToBack(xMotorCommandQueue, (void *) &char_array, 5) == errQUEUE_FULL) {
						//TODO: Warn about Full xMotorCommandQueue
						}
					}
				//clear buffers
				memset(buf,0,sizeof(buf));
				ptr=0;
				//toggleDebug(); //Gets the right DELIMITER and gets the right MESSAGE_LEN
				//TODO: Look into pointer issue and memAlloc issue
				//strcpy(char_array, msg.c_str());
				//Serial.write(char_array); //WT*: only works if toggleDebug() is above this ???
				//Serial.write('\n');
				//if (char_array[2] == '3') toggleDebug();
				}
			}
		//reminder not to send '\0' over serial.
		//scratch that 115200 baud has too much noise, 9600. Therefore Time taken for 6 bytes = 5.6ms~ , so we can send a heartbeat once every 10 ms, we can read
		//the serial buffer once every 10 ms as well
		vTaskDelayUntil(&lastWakeTime, 10);	//@9600, characters come ~1ms apart
	}
}


void xTaskMotor(void *p) {
	char command[MESSAGE_LEN-1];
	char status = STATIONARY;
	for (;;) {
		xQueueReceive(xMotorCommandQueue, (void *) command, portMAX_DELAY);

		//if(command[3]=='3') toggleDebug(); //SANITY_CHECK: Proves the msgQ is working

		//Parse Command

		// if-elseif-else
		if (command[4] == 'S' ){
			stop();
			status=STATIONARY;
		}
		else if (command[4] == 'M'){
			setpower(command[0],command[1],command[2],command[3]);
			status=MOVING;
		}
		else if (command[4] == 'W' && ((command[1]+command[2]+command[3]+command[4] == 4))){

		}
		// Call Appropriate functions from MotorDriver.h
		// change Status if needed
		//status = 1; //placeholder for testing
		if (xQueueSendToBack(xLEDStatusQueue, &status, 5) != pdTRUE) {
			//TODO: Warn about Full xLEDStatusQueue for too long
		}
		if (xQueueSendToBack(xAudioStatusQueue, &status, 5) != pdTRUE) {
			//TODO: Warn about Full xAudioStatusQueue for too long
		}
	}
}


// leastPriority
void xTaskAudio(void *p) {
	TickType_t xLastWakeTime = xTaskGetTickCount();
	int status = STATIONARY;
	unsigned int babySharkPointer = 0;
	unsigned int winMusicPointer = 0;
	for(;;) {
		//TODO: [BUG] Nothing will start until some command sent
		//xQueueReceive(xAudioStatusQueue, &status, 0); //non-blocking
		if ( (status == STATIONARY) || (status == MOVING) || (status == STATIONARY)){;
			if(playBabyShark(babySharkPointer++)){
				babySharkPointer = 0;
			}
		}
		else if ((status == WON)) {
			;
		}
		vTaskDelayUntil(&xLastWakeTime, TIMEPERNOTE);
	}
}

// leastPriority
void xTaskLED(void *p) {
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xRedLEDMovingFreq = 500;
	const TickType_t xRedLEDStationaryFreq = 250;
	int status = STATIONARY;

	for(;;) {
		//TODO: [BUG] Nothing will start until some command sent
		xQueueReceive(xLEDStatusQueue, &status, 0); //non-blocking
		//if (status == 1) toggleDebug(); //SANITY_CHECK: Proves msgQ working

		if (status==MOVING) {
			greenRunning();	//??CANTHEYBESYNCHRONOUS?
			toggleRed();
			vTaskDelayUntil(&xLastWakeTime, xRedLEDMovingFreq);
			//can you use this inside scheduled tasks only
		}
		else if (status==STATIONARY) {
			greenOn();
			toggleRed();
			vTaskDelayUntil(&xLastWakeTime, xRedLEDStationaryFreq);
		}
		else {
			//TODO: Warn Received Wrong status
		}
	}
}

void setup() {
	setupMotors();
	setupBluetooth();
	setupLED();
	setupAudio();
	//Serial.begin(115200); //for Serial xTaskDebugger
}

void loop() {
	//putting a low priority might always block these 3
	//xTaskCreate(xTaskDebugger, "TaskDebugger", STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(xTaskLED, "TaskLED", STACK_SIZE, NULL, 2, NULL);
	xTaskCreate(xTaskAudio, "TaskAudio", STACK_SIZE, NULL, 2, NULL);

	// Putting TaskBluetooth higher than xTaskMotor might be dangerous
	// Solution1: xTaskBluetooth is MUTEX with Serial RX ISR
	// Solution2: xTaskMotor and xTaskBletooth hold same MUTEX (solution by Priority Inversion)
	// Solution3: Since xTaskMotor depends on xTaskBluetooth sending data, put xTaskMotor higher
	xTaskCreate(xTaskMotor, "TaskMotor", STACK_SIZE, NULL, 3, NULL);
	xTaskCreate(xTaskBluetooth, "TaskBluethooth", STACK_SIZE, NULL, 4, NULL);

	//create scheduler, RMS right now
	vTaskStartScheduler();
}

