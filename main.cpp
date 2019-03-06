#include <Arduino.h>
#include <avr/io.h>
#include <String.h>
#include <string.h>
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
#define STATIONARY 0
#define MOVING 1
#define WON 2

//TODO: ReEvaluate
#define MESSAGE_LEN 5

// Poster: xTaskBluetooth
// Taker: xTaskMotor
// Single item Queue because we should ensure xTaskMotor is never too late in responding.
// Ideally, make sure item in queue is taken care of before next item comes in (t>MESSAGE_LENms)
QueueHandle_t xMotorCommandQueue = xQueueCreate(1, sizeof(int)*MESSAGE_LEN);

// Poster: xTaskMotor
// Taker: xTaskLED and xTaskAudio respectively
// Single item Queue again because should be responsive enough
QueueHandle_t xLEDStatusQueue = xQueueCreate(1, sizeof(int));
QueueHandle_t xAudioStatusQueue = xQueueCreate(1, sizeof(int));

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
	String msg = "";
	for (;;) {
		char c = receiveChar();

		// if Delimiter,
		// since HC-06 doesn't use /r or /n
		if (c == '.') {
			//TODO: [BUG] 	For first ever message, first char becomes non-printable
			// 				For subsequent messages, first char is non-print, but all chars reserved.
			if (msg.length() == MESSAGE_LEN) {
				//TODO: Look into pointer issue and memAlloc issue
			    char char_array[MESSAGE_LEN+1];
			    strcpy(char_array, msg.c_str());
				if (xQueueSendToBack(xMotorCommandQueue, (void *) char_array, 5) != pdTRUE) {
					//TODO: Warn about Full xMotorCommandQueue
				}
			} else {
				//TODO: Warn about wrong length of reception
			}
			if(msg[3]=='3') toggleDebug();
			msg = "";
		}

		if (c != '\0') {
			msg += c;
		}
		vTaskDelayUntil(&lastWakeTime, 1);	//@9600, characters come ~1ms apart
	}
}


void xTaskMotor(void *p) {
	char command[MESSAGE_LEN+1];
	int status;
	for (;;) {
		xQueueReceive(xMotorCommandQueue, (void *) command, portMAX_DELAY);

		//if(command[3]=='3') toggleDebug(); //SANITY_CHECK: Proves the msgQ is working

		//Parse Command

		// if-elseif-else
		// Call Appropriate functions from MotorDriver.h
		// change Status if needed
		status = 1; //placeholder for testing
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
	int status = STATIONARY;
	for(;;) {
		//TODO: [BUG] Nothing will start until some command sent
		xQueueReceive(xAudioStatusQueue, &status, 0); //non-blocking

		//if (status == 1) toggleDebug(); //SANITY_CHECK: Proves msgQ working

		//TODO: This won't work. @Sean please see.
		if (status==WON) playVictorySong();
		else playBabyShark();
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
	//Serial.begin(9600); //for Serial xTaskDebugger
}

void loop() {
	//putting a low priority might always block these 3
	//xTaskCreate(xTaskDebugger, "TaskDebugger", STACK_SIZE, NULL, 1, NULL);
	//xTaskCreate(xTaskLED, "TaskLED", STACK_SIZE, NULL, 2, NULL);
	//xTaskCreate(xTaskAudio, "TaskAudio", STACK_SIZE, NULL, 2, NULL);

	// Putting TaskBluetooth higher than xTaskMotor might be dangerous
	// Solution1: xTaskBluetooth is MUTEX with Serial RX ISR
	// Solution2: xTaskMotor and xTaskBletooth hold same MUTEX (solution by Priority Inversion)
	// Solution3: Since xTaskMotor depends on xTaskBluetooth sending data, put xTaskMotor higher
	//xTaskCreate(xTaskMotor, "TaskMotor", STACK_SIZE, NULL, 3, NULL);
	xTaskCreate(xTaskBluetooth, "TaskBluethooth", STACK_SIZE, NULL, 4, NULL);

	//create scheduler, RMS right now
	vTaskStartScheduler();
}

