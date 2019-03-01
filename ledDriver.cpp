#include <Arduino.h>
#include <avr/io.h>
#include "ledDriver.h"

#define DEBUG_LED 13
#define PIN_RED 4
#define PIN_GREEN 7

int redOn = 0;
int debugOn = 0;

// Think about how to extend pins for Green LED
// I'm thinking shiftRegister plus Mode
void setupLED() {
	pinMode(PIN_RED, OUTPUT);
	pinMode(PIN_GREEN, OUTPUT);
	pinMode(DEBUG_LED, OUTPUT);
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
	redOn = !redOn;
	digitalWrite(PIN_RED, redOn);
}

//!!! Use at <= 1 line to preserve Sanity during SANITY_CHECKS
void toggleDebug() {
	debugOn = !debugOn;
	digitalWrite(DEBUG_LED, debugOn);
}
