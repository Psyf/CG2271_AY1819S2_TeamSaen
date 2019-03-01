/*
 * reference: http://www.martyncurrey.com/arduino-and-hc-06-zs-040/
 * Name = teleChickenBT [1234]
 * 9600 w/ (no line ending); 5V TX/RX works
 * Device receives data properly with Bluetooth Terminal
 * To send data from Device, set to "Both NL & CR"
 */

#include <Arduino.h>
#include <avr/io.h>
#include "SoftwareSerial.h"
#include "bluetoothDriver.h"
#include <FreeRTOS.h>
#include <task.h>
#include <String.h>

// Software Serial is poor beyond 9600, is blocking too, takes too much time
// Consider switching to Hardware+Interrupt
SoftwareSerial btSerial(2, 3); // RX, TX

void setupBluetooth() {
	btSerial.begin(9600);
}

char receiveChar() {
	  if (btSerial.available()) {
		  return (char) btSerial.read();
	  }
	  else return '\0';
}

void sendMessage(char *msgFromDevice) {
	btSerial.write(msgFromDevice);
}

