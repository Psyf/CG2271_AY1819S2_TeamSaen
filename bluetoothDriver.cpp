/*
 * reference: http://www.martyncurrey.com/arduino-and-hc-06-zs-040/
 * Name = teleChickenBT [1234]
 * 9600 w/ (no line ending); 5V TX/RX works
 * Device receives data properly with Bluetooth Terminal
 * To send data from Device, set to "Both NL & CR"
 */

#include <Arduino.h>
#include <avr/io.h>
#include "bluetoothDriver.h"
#include <FreeRTOS.h>
#include <task.h>
#include <String.h>
#include "ledDriver.h"

void setupBluetooth() {
	Serial.begin(9600);
}

char receiveChar() {
	  if (Serial.available()) {
		  //toggleDebug(); //Tested: SerialIsAvailable.
		  return (char) Serial.read();
	  }
	  else return '\0';
}

void sendMessage(char *msgFromDevice) {
	Serial.write(msgFromDevice);
}

