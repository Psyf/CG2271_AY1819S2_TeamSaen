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

volatile int _status = 0;

// taskFunctions with logic inside

//HighestPriority
//Ideally being handled by an interrupt
//xTaskBluetooth() {
//	for (;;) {
	//	if msg.ready()
	// 		readMsg()
	// 		parseMsg()
	// 		changeStatus()
	// 		!!!Chance to think about interTaskCommunication!!!
//	}
//}

// SecondHighestPriority?
//xTaskMotor() {
//for(;;) {
//	if status=[F/B/S/L/R]
//		callApproppriateFunction
//}
//}

//SecondLast
//xTaskAudio() {
//	for(;;) {
	//	if (status="won") playVictorySong();
	//	else playBabyShark();
//	}
//}

// leastPriority
// where in the project structure to put the tasks?
void xTaskLED(void *p) {
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xRedLEDMovingFreq = 500;
	const TickType_t xRedLEDStationaryFreq = 250;
	for(;;) {	// why not use while(1)?
		if (_status=MOVING) {
			greenRunning();	//??CANTHEYBESYNCHRONOUS?
			toggleRed();
			vTaskDelayUntil(&xLastWakeTime, xRedLEDMovingFreq);
		}
		else if (_status=STATIONARY) {
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
}

void loop() {
	//create tasks
	xTaskCreate(xTaskLED, "TaskLED", STACK_SIZE, NULL, 1, NULL);
	//create scheduler
}
