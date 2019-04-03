#include <Arduino.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "ledDriver.h"

#define DEBUG_LED 3
#define PIN_RED 2
#define CLK A0 //IC pin 11 	ARDUINO 12<-- OLD  NEW -->4
#define LATCH A1 //IC pin 12 ARDUINO 8<-- OLD  NEW -->5
#define DATA A2 //IC pin 14  ARDUINO 13<-- OLD  NEW -->7

#define LEDPATTERN0 0
#define LEDPATTERN1 24
#define LEDPATTERN2 60
#define LEDPATTERN3 126
#define LEDPATTERN4 255

char redOn = 0;
char debugOn = 0;

//shifted pattern here. I think it think putting it in the task was creating some issues. Probably a stack overflow.
//alternate shift of constants to .text memory is using progmem. However requires special functions to read from PROGMEM
//so id really that just put everything in a switch case statement.
const PROGMEM uint8_t pattern[]=
{
		LEDPATTERN0, //0000 0000
		LEDPATTERN1, //0001 1000
		LEDPATTERN2, //0011 1100
		LEDPATTERN3, //0111 1110
		LEDPATTERN4, //1111 1111
};

// Think about how to extend pins for Green LED
// I'm thinking shiftRegister plus Mode
void setupLED() {
	pinMode(PIN_RED, OUTPUT);
	pinMode(DEBUG_LED, OUTPUT);
	pinMode(CLK,OUTPUT);
	pinMode(LATCH,OUTPUT);
	pinMode(DATA,OUTPUT);
}

// writes 8 bits of data to the SR. 8 bits specified by val
void greenPatternOut(uint8_t val){
	digitalWrite(CLK,LOW);
	digitalWrite(LATCH,LOW);
	shiftOut(DATA,CLK,MSBFIRST, val);
	digitalWrite(LATCH,HIGH);
}
// Lit when bot is moving | for use with preset
void greenOn(uint8_t patternPresetIndex) {
	greenPatternOut(pgm_read_byte_near(pattern + patternPresetIndex));
}


// Used for both running and stationary
// albeit different frequencies
bool toggleRed(bool state) {
	digitalWrite(PIN_RED, state);
	return 1 - state;
}

//!!! Use at <= 1 line to preserve Sanity during SANITY_CHECKS
void toggleDebug() {
	debugOn = !debugOn;
	digitalWrite(DEBUG_LED, debugOn);
}
