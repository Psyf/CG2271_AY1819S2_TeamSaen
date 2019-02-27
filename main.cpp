#include <Arduino.h>
#include <avr/io.h>
#include <FreeRTOS.h>
#include <task.h>
#define STACK_SIZE 200

#include "audioDriver.h"
#include "ledDriver.h"
#include "motorDriver.h"
#include "bluetoothDriver.h"

#define STATIONARY 0
#define MOVING 1
#define WON 2

volatile int _status = 0;

// taskFunctions with logic inside

//HighestPriority
//Ideally being handled by an interrupt
void xTaskBluetooth(void *p) {
	TickType_t lastWakeTime = xTaskGetTickCount();
	for (;;) {
		// Serial.println not fast enough, use Serial.write(byte)
		char* msgFromApp = receiveCommand();
		if (msgFromApp[0]!='\0') {
			//Parse(msgFromApp)
			//TakeAction
		}
	}
	 		//!!!Chance to think about interTaskCommunication!!!
}


// SecondHighestPriority?
//xTaskMotor() {
//for(;;) {
//	if status=[F/B/S/L/R]
//		callApproppriateFunction
//}
//}

// leastPriority
void xTaskAudio(void *p) {
	// do I need to vTaskDelay?
	// else fills up every non-contested time
	// energy cost?
	for(;;) {
		if (_status==WON) playVictorySong();
		else playBabyShark();
	}
}

// leastPriority
// where in the project structure to put the tasks?
// todo: test
void xTaskLED(void *p) {
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xRedLEDMovingFreq = 500;
	const TickType_t xRedLEDStationaryFreq = 250;
	for(;;) {	// why not use while(1)?
		if (_status==MOVING) {
			greenRunning();	//??CANTHEYBESYNCHRONOUS?
			toggleRed();
			vTaskDelayUntil(&xLastWakeTime, xRedLEDMovingFreq);
			//can you use this inside scheduled tasks only
		}
		else if (_status==STATIONARY) {
			greenOn();
			toggleRed();
			vTaskDelayUntil(&xLastWakeTime, xRedLEDStationaryFreq);
		}
		else {
			// can we somehow log when _status is wrong?
		}
	}
}

void setup() {
	setupMotors();
	setupBluetooth();
	setupLED();
	setupAudio();

	Serial.begin(9600);
}

void loop() {

	//create tasks
	// !! Creating more than 1 task stops Serial.
	//xTaskCreate(xTaskLED, "TaskLED", STACK_SIZE, NULL, 1, NULL);
	//xTaskCreate(xTaskAudio, "TaskAudio", STACK_SIZE, NULL, 1, NULL);
	// Creating BTTask also ruins Serial for some reason.
	xTaskCreate(xTaskBluetooth, "TaskBluethooth", STACK_SIZE, NULL, 3, NULL);
	//create scheduler, RMS right now
	vTaskStartScheduler();
}

