#include <Arduino.h>
#include <avr/io.h>
#include "ledDriver.h"

#define DEBUG_LED 2
#define PIN_RED 4
#define PIN_GREEN 7
#define CLK 12 //IC pin 11
#define LATCH 8 //IC pin 12
#define DATA 13 //IC pin 14

int redOn = 0;
int debugOn = 0;

// Think about how to extend pins for Green LED
// I'm thinking shiftRegister plus Mode
void setupLED() {
	pinMode(PIN_RED, OUTPUT);
	pinMode(PIN_GREEN, OUTPUT);
	pinMode(DEBUG_LED, OUTPUT);
	pinMode(CLK,OUTPUT);
	pinMode(LATCH,OUTPUT);
	pinMode(DATA,OUTPUT);
}

// Lit when bot is moving
void greenOn(uint8_t val) {
	digitalWrite(LATCH,LOW);
	shiftOut(DATA,CLK,MSBFIRST,val);
	digitalWrite(LATCH,HIGH);
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
