#define __AVR_ATmega328P__
#define F_CPU 16000000L
#include <Arduino.h>
#include <avr/io.h>
//#include <String.h>
//#include <string.h>
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
#define INIT 'I'
#define QUIET 'Q'

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

			if (c != '\0') {
					buf[ptr++] = c;
			}

			// if Delimiter,
			// since HC-06 doesn't use /r or /n
			if (c == '.' ) {
				if (ptr == MESSAGE_LEN){ //send to Q
					char char_array[MESSAGE_LEN-1]; // stripping the .
					memcpy(char_array,buf, sizeof(char_array));
					if (xQueueSendToBack(xMotorCommandQueue, (void *) &char_array, 5) == errQUEUE_FULL) {
						//TODO: Warn about Full xMotorCommandQueue
						}
					//TODO: if INIT MEssage, don't do anything for 1 second
					//TODO: Has to be implemented app-side too
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
	char status= STATIONARY;
	char oldStatus = STATIONARY;

	for (;;) {
		if (xQueueReceive(xMotorCommandQueue, (void *) command, 3000) == pdFALSE) {
			status = STATIONARY; 	//emergency breaks, if we  haven't received anything
									// from the app in 300 ms (~ 15 missed packets)
		}
		else {
			status = command[4];
		}

		if (status == STATIONARY ){
			stop();
		}
		else if (status == MOVING || status == QUIET ){
			setpower(command[0],command[1],command[2],command[3]);
		}
		else if (status == WON && ((command[1]+command[2]+command[3]+command[4] == 4))){
			setpower(command[0],command[1],command[2],command[3]);
		}

		// LED and Audio change ONLY IF status is changed
		// maximum blocking period below here is 6 ticks. Therefore this task should unblock frequently enough to consume
		// the bluetooth queue every ~6 ms, assuming execution time is <<< 1 tick (which it should be, even including the PWM intterupts)
		if (oldStatus != status) {
			if (status!=QUIET){
				if ((xQueueSendToBack(xLEDStatusQueue, &status, 3) != pdTRUE) ) {
					//TODO: Warn about Full xLEDStatusQueue for too long
				}
			}
			if (xQueueSendToBack(xAudioStatusQueue, &status, 3) != pdTRUE) {
				//TODO: Warn about Full xAudioStatusQueue for too long
			}
			oldStatus = status;
		}
	}
}

void xTaskAudio(void *p) {
	TickType_t xLastWakeTime = xTaskGetTickCount();
	char song = BBYSHARK;
	char songEnable = 1;
	int status = STATIONARY;
	unsigned int musicPointer = 0;
	for(;;) {
		xQueueReceive(xAudioStatusQueue, &status, 0); //non-blocking
		if (status == QUIET) {
			songEnable = 1-songEnable;
		}
		if (songEnable){
			if ( statusToSong(status) != song){
				song = statusToSong(status);
				musicPointer=0;
			}

			if ( song == BBYSHARK){
				if(playBabyShark(musicPointer++)){
					musicPointer = 0;
				}
			}
			else if (song == WINSONG) {
				if(playVictorySong(musicPointer++)){
					musicPointer = 99;
				}
			}
			else if (song == CONNECTSONG){
				if(playInitSong(musicPointer++)){
					musicPointer = 99;
				}
			}
		}
		else {
			myNoTone();
		}
		vTaskDelayUntil(&xLastWakeTime, TIMEPERNOTE);
	}
}

void xTaskLED(void *p) {
	const TickType_t xRedLEDMovingFreq = 500;
	const TickType_t xRedLEDStationaryFreq = 250;
	const uint8_t  MAXCOUNT = 4;
	char status = STATIONARY;
	uint8_t index = 0;
	bool redLEDState = 0;
	TickType_t xLastWakeTime = 0;
	for(;;) {
		xQueueReceive(xLEDStatusQueue, &status, 0); //non-blocking
		if (status==MOVING) {
			greenOn(index++);
			if (index > MAXCOUNT) {
				index=0;
			}
			redLEDState = toggleRed(redLEDState);
			vTaskDelayUntil(&xLastWakeTime, xRedLEDMovingFreq);
		}
		else if (status==STATIONARY  || status == WON) {
			greenPatternOut(B11111111);
			redLEDState = toggleRed(redLEDState);
			vTaskDelayUntil(&xLastWakeTime, xRedLEDStationaryFreq);
		}
		else if (status == INIT) {	//2 green flashes
			greenPatternOut(B00000000);
			redLEDState = toggleRed(redLEDState);
			vTaskDelayUntil(&xLastWakeTime, 250);
			greenPatternOut(B11111111);
			redLEDState = toggleRed(redLEDState);
			vTaskDelayUntil(&xLastWakeTime, 250);
			greenPatternOut(B00000000);
			redLEDState = toggleRed(redLEDState);
			vTaskDelayUntil(&xLastWakeTime, 250);
			greenPatternOut(B11111111);
			redLEDState = toggleRed(redLEDState);
			vTaskDelayUntil(&xLastWakeTime, 250);
			greenPatternOut(B00000000);
			redLEDState = toggleRed(redLEDState);
			status = STATIONARY;
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

	xTaskCreate(xTaskLED, "TaskLED", 200, NULL, 1, NULL);
	xTaskCreate(xTaskAudio, "TaskAudio", 300, NULL, 2, NULL);

	// Putting TaskBluetooth higher than xTaskMotor might be dangerous
	// Solution1: xTaskBluetooth is MUTEX with Serial RX ISR
	// Solution2: xTaskMotor and xTaskBletooth hold same MUTEX (solution by Priority Inversion)
	// Solution3: Since xTaskMotor depends on xTaskBluetooth sending data, put xTaskMotor higher
	xTaskCreate(xTaskMotor, "TaskMotor", 100, NULL, 3, NULL);
	xTaskCreate(xTaskBluetooth, "TaskBluethooth", 100, NULL, 4, NULL);

	//create scheduler, RMS right now
	vTaskStartScheduler();
}

