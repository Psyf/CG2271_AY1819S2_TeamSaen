/*
 * reference: http://www.martyncurrey.com/arduino-and-hc-06-zs-040/
 * Name = teleChickenBT [1234]
 * 9600 w/ (no line ending); 5V TX/RX works
 * Device receives data properly with Bluetooth Terminal
 * To send data from Device, set to "Both NL & CR"
 */

#include <Arduino.h>
#include <avr/io.h>
#include "SoftwareSerial.h"	//fix or use hardwareSerial
#include "bluetoothDriver.h"
#include <FreeRTOS.h>
#include <task.h>
#include <String.h>

SoftwareSerial btSerial(12, 13); // RX, TX

void setupBluetooth() {
	btSerial.begin(9600);
}

//right now can receive strictly less than 5 chars
// btSerial.available = 0 within a String.
// The code might be too fast, or being preempted?
//
char* receiveCommand() {
	  TickType_t lastWakeTime = xTaskGetTickCount();
	  char* msgFromApp = '\0';
	  int i;
	  if (btSerial.available()) {
		  i=0;
		  char c;
		  while (btSerial.available() > 0) {
			  c = (char) btSerial.read();
			  msgFromApp[i++] = c;
			  Serial.write(msgFromApp[i-1]);
		  }
		  msgFromApp[i] = '\0';
		  Serial.write('.');
		  return msgFromApp;
	  }
	  else {
		  return '\0';
	  }

}

void sendMessage(char *msgFromDevice) {
	btSerial.write(msgFromDevice);
}

