#include <Arduino.h>
#include <avr/io.h>
#include "audioDriver.h"
#include <FreeRTOS.h>
#include <task.h>
#define STACK_SIZE 200

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

//leastPriority
/*xTaskLED() {
	for(;;) {
		if (status=moving) {
			greenRunning();	??CANTHEYBESYNCHRONOUS?
			toggleRed();
			sleepFor250;
		}
		else {
			greenOn();
			toggleRed();
			sleepFor500();
		}
	}
}*/

void setup() {
	setupMotors();
	setupBluetooth();
	setupLED();
	setupAudio();
}

void loop() {
	//create tasks
	//create scheduler
}
