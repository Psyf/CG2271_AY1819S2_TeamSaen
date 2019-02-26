#include <Arduino.h>
#include <avr/io.h>
#include "ledDriver.h"

#define PIN_RED 3
#define PIN_GREEN 4

int redOn = 0;

// Think about how to extend pins for Green LED
// I'm thinking shiftRegister plus Mode
void setupLED() {
	pinMode(PIN_RED, OUTPUT);
	pinMode(PIN_GREEN, OUTPUT);
}

// Lit when bot is moving
void greenOn() {
	return;
}

// Off when bot starts moving again.
void greenOff() {
	return;
}

// Lit when bot is moving
void greenRunning() {
	greenOff();
}

// Used for both running and stationary
// albeit different frequencies
void toggleRed() {
	digitalWrite(PIN_RED, redOn);
	redOn = !redOn;
}

