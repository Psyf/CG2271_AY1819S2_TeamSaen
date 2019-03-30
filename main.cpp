#include <Arduino.h>
#include <avr/io.h>
#include <String.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#define STACK_SIZE 200

// Include drivers for Devices
#include "audioDriver.h"
#include "ledDriver.h"
#include "motorDriver.h"
#include "bluetoothDriver.h"

// Status Definitions
#define STATIONARY 'S'
#define MOVING 'M'
#define WON 'W'

// Fixed Message_Len depending on Protocol
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
	char status;
	char oldStatus = 0;

	for (;;) {
		xQueueReceive(xMotorCommandQueue, (void *) command, portMAX_DELAY);
		status = command[4];

		if (status == 'S' ){
			stop();
		}
		else if (status == 'M'){
			setpower(command[0],command[1],command[2],command[3]);
		}
		else if (status == 'W' && ((command[1]+command[2]+command[3]+command[4] == 4))){
			// do something
		}

		// LED and Audio change ONLY IF status is changed
		if (oldStatus != status) {
			if (xQueueSendToBack(xLEDStatusQueue, &status, 5) != pdTRUE) {
				//TODO: Warn about Full xLEDStatusQueue for too long
			}
			if (xQueueSendToBack(xAudioStatusQueue, &status, 5) != pdTRUE) {
				//TODO: Warn about Full xAudioStatusQueue for too long
			}
			oldStatus = status;
		}
	}
}

void xTaskAudio(void *p) {
	int status = STATIONARY;
	for(;;) {
		xQueueReceive(xAudioStatusQueue, &status, 0); //non-blocking

		if (status == WON) playVictorySong();
		else playBabyShark();
	}
}

void xTaskLED(void *p) {
	const TickType_t xRedLEDMovingFreq = 500;
	const TickType_t xRedLEDStationaryFreq = 250;
	int status = STATIONARY;

	byte pattern[]=
	{
	B00000000,
	B00011000,
	B00111100,
	B01111110,
	B11111111,
	};

	int index =0;
	int count=sizeof(pattern);

	TickType_t xLastWakeTime = xTaskGetTickCount();
	for(;;) {
		xQueueReceive(xLEDStatusQueue, &status, 0); //non-blocking

		if (status==MOVING) {
			index++;
			if(index>=count) index=0;
			greenOn(pattern[index]);
			toggleRed();
			vTaskDelayUntil(&xLastWakeTime, xRedLEDMovingFreq);
		}
		else if (status==STATIONARY) {
			greenOn(B11111111);
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
}

void loop() {
	xTaskCreate(xTaskLED, "TaskLED", STACK_SIZE, NULL, 1, NULL);
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

