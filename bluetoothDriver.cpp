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
#include <String.h>

SoftwareSerial btSerial(12, 13); // RX, TX

void setupBluetooth() {
	btSerial.begin(9600);
}

// Have to fix this
char* receiveCommand() {
	  char* msgFromApp;
	  if (btSerial.available()) {
		  Serial.println(btSerial.read());
	  } else {
		  msgFromApp = (char*) "None";
	  }
	  return msgFromApp;
}

void sendMessage(char *msgFromDevice) {
	btSerial.write(msgFromDevice);
}

