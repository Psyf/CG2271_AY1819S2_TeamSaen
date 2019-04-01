#include <Arduino.h>
#include <avr/io.h>
#include "ledDriver.h"

#define DEBUG_LED 2
#define PIN_RED 3
#define CLK 4 //IC pin 11 	ARDUINO 12<-- OLD  NEW -->4
#define LATCH 5 //IC pin 12 ARDUINO 8<-- OLD  NEW -->5
#define DATA 7 //IC pin 14  ARDUINO 13<-- OLD  NEW -->7

int redOn = 0;
int debugOn = 0;

// Think about how to extend pins for Green LED
// I'm thinking shiftRegister plus Mode
void setupLED() {
	pinMode(PIN_RED, OUTPUT);
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
