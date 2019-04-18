#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include <Arduino.h>
#include <avr/io.h>

#ifndef STATIONARY
#define STATIONARY 'S'
#define MOVING 'M'
#define WON 'W'
#define INIT 'I'
#define QUIET 'Q'
#endif

#include "audioDriver.h"

//const char BABYSHARKMAIN[BABYSHARKMAINLEN] = {NOTE_D0,NOTE_D0,NOTE_D0,NOTE_D0,NOTE_E0,NOTE_E0,NOTE_E0,NOTE_E0,NOTE_G0,NOTE_X,NOTE_X,NOTE_X,NOTE_G0,NOTE_X,NOTE_G0,NOTE_X,NOTE_X,NOTE_X,NOTE_G0,NOTE_X,NOTE_G0,NOTE_X,NOTE_X,NOTE_X,NOTE_G0,NOTE_X,NOTE_G0,NOTE_X,NOTE_X,NOTE_X};
//const char CONNECTIONJINGLE[] = {NOTE_G0,NOTE_G0,NOTE_D0,NOTE_C0,NOTE_G0,NOTE_G0,NOTE_D0,NOTE_D0,NOTE_X,NOTE_X,NOTE_X,NOTE_X};
//const char VICTORYJINGLE[] = {NOTE_E0,NOTE_E0,NOTE_E0,NOTE_E0,NOTE_X,NOTE_X,NOTE_E0,NOTE_X,NOTE_E0,NOTE_X,NOTE_F0,NOTE_X,NOTE_G0,NOTE_X,NOTE_A0,NOTE_A0,NOTE_A0,NOTE_A0,NOTE_X,NOTE_X,NOTE_X,NOTE_X};

// import the notes as freq from the internet
void setupAudio(){
  //SET OC2A to output
  DDRD |= (1<<DDD6);

  //setup TCCR2A - CTC MODE AND TOGGLE ON COMPARE MATCH (HERE ITS OC0A is DISCONNECTED FIRST, SET COM0A0 to toggle mode later)
  TCCR0A &= ~(1<<COM0A1 | 1<<WGM00 | 1<<COM0A0 | 1<<COM0A0);
  TCCR0A |= ( 1<<WGM01 );

  //setup TCCR2B - PRESCALAR SELECTION->256
  TCCR0B &= ~(1<<FOC2A | 1<<FOC2B | 1<<WGM02 | 1<<CS01 | 1<<CS00);
  TCCR0B |= (1<<CS02);


  //DISABLES TIMER INTTERUPTS
  TIMSK0 &= ~(1<<OCIE0B | 1<<OCIE0A | 1<<TOIE2);

  //set OCR0A to 255
  OCR0A = (uint8_t) 255;

}

void myNoTone(){
  TCCR0A &= ~(1<<COM0A0); //disconnnect OCR2A
}


void myTone(unsigned int count){
  if (count > 0){
     OCR0A = (uint8_t) count;
     TCCR0A |= (1<<COM0A0); //toggle on compare match
  }
  else {
    myNoTone();
  }

}

void setPiezo(unsigned int freq){
  if (freq != 0){
    myTone(freq);
  }
  else{
    myNoTone();
  }
}

char statusToSong(char status){
	if ( (status == STATIONARY) || (status == MOVING) || (status == STATIONARY)){
			return BBYSHARK;
			}
		else if ((status == WON)) {
			return WINSONG;
		}
		else if (status == INIT) {
			return CONNECTSONG;
		}
	return BBYSHARK;
}

//alternative way of storing data to .text. Don't worry, wrote a python script to generate the switch cases. not sure if this would be faster tho
//than accessing progmem. either way, i like this way better, simply because i get to use my script. If you're grading this, pls do not take offence, the LEDs patterns are stored
//using PROGMEM variable modifier, and read from PROGMEM via pgm_read_byte_near(). In fact, progmem is probably faster, but i like my python script. Also the switch cases look cool.
int playBabySharkMAIN(unsigned int p){
	switch(p){
	    case 0:
	    case 1:
	    case 2:
	    case 3:
	        setPiezo(NOTE_D0);
	        return 0;
	    case 4:
	    case 5:
	    case 6:
	    case 7:
	        setPiezo(NOTE_E0);
	        return 0;
	    case 8:
	    case 12:
	    case 14:
	    case 18:
	    case 20:
	    case 24:
	    case 26:
	        setPiezo(NOTE_G0);
	        return 0;
	    case 9:
	    case 10:
	    case 11:
	    case 13:
	    case 15:
	    case 16:
	    case 17:
	    case 19:
	    case 21:
	    case 22:
	    case 23:
	    case 25:
	    case 27:
	    case 28:
	    case 29:
	        setPiezo(NOTE_X);
	        return 0;
	    default:
	        myNoTone();
	        return 1;
	}
}

// plays baby shark at desired part of the song
// p ranges from 0 to 101 for a total 102 notes.
// returns TRUE if p > than song length (ie the end of the song is reached)
bool playBabyShark(unsigned int p) {
	if (p < 90) {
		playBabySharkMAIN(p%30);
	}
	else if (p == 98){
		setPiezo(NOTE_F0);
	}
	else if (p < 102){
		playBabySharkMAIN(p%30);
	}
	else {
		return 1;
	}
	return 0;
}



bool playInitSong(unsigned int p) {
	switch(p){
	    case 8:
	    case 9:
	        setPiezo(NOTE_E0);
	        return 0;
	    case 42:
	    case 43:
	    case 48:
	    case 49:
	        setPiezo(NOTE_F0);
	        return 0;
	    case 6:
	    case 7:
	    case 16:
	    case 17:
	    case 22:
	    case 23:
	    case 24:
	    case 25:
	    case 34:
	    case 35:
	    case 36:
	    case 37:
	    case 40:
	    case 41:
	    case 44:
	    case 45:
	    case 50:
	    case 51:
	    case 52:
	    case 53:
	        setPiezo(NOTE_G0);
	        return 0;
	    case 32:
	    case 33:
	    case 38:
	    case 39:
	        setPiezo(NOTE_A0);
	        return 0;
	    case 4:
	    case 5:
	    case 14:
	    case 15:
	    case 18:
	    case 19:
	    case 20:
	    case 21:
	    case 26:
	    case 27:
	    case 28:
	    case 29:
	    case 54:
	    case 55:
	        setPiezo(NOTE_B0);
	        return 0;
	    case 30:
	    case 31:
	    case 46:
	    case 47:
	    case 62:
	    case 63:
	        setPiezo(NOTE_X);
	        return 0;
	    case 0:
	    case 1:
	    case 2:
	    case 3:
	    case 10:
	    case 11:
	    case 12:
	    case 13:
	    case 56:
	    case 57:
	        setPiezo(NOTE_D1);
	        return 0;
	    case 58:
	    case 59:
	        setPiezo(NOTE_E1);
	        return 0;
	    case 60:
	    case 61:
	        setPiezo(NOTE_F1);
	        return 0;
	    default:
	        myNoTone();
	        return 1;
	}
}

// make victory song using notes


bool playVictorySong(unsigned int p) {
	switch(p){
	    case 48:
	    case 49:
	        setPiezo(NOTE_D0);
	        return 0;
	    case 44:
	    case 45:
	        setPiezo(NOTE_E0);
	        return 0;
	    case 26:
	    case 27:
	    case 30:
	    case 31:
	    case 42:
	    case 43:
	    case 46:
	    case 47:
	    case 54:
	    case 55:
	    case 58:
	    case 59:
	    case 70:
	    case 71:
	        setPiezo(NOTE_F0);
	        return 0;
	    case 24:
	    case 25:
	    case 28:
	    case 29:
	    case 40:
	    case 41:
	    case 52:
	    case 53:
	    case 56:
	    case 57:
	    case 68:
	    case 69:
	    case 72:
	    case 73:
	        setPiezo(NOTE_G0);
	        return 0;
	    case 34:
	    case 35:
	    case 38:
	    case 39:
	    case 62:
	    case 63:
	    case 66:
	    case 67:
	        setPiezo(NOTE_A0);
	        return 0;
	    case 0:
	    case 2:
	    case 4:
	    case 6:
	    case 14:
	    case 15:
	    case 20:
	    case 21:
	    case 76:
	    case 77:
	    case 78:
	        setPiezo(NOTE_C1);
	        return 0;
	    case 1:
	    case 3:
	    case 5:
	    case 7:
	    case 10:
	    case 13:
	    case 16:
	    case 19:
	    case 22:
	    case 23:
	    case 50:
	    case 51:
	    case 79:
	    case 80:
	    case 81:
	        setPiezo(NOTE_X);
	        return 0;
	    case 8:
	    case 9:
	        setPiezo(NOTE_G0_SHARP);
	        return 0;
	    case 11:
	    case 12:
	    case 17:
	    case 18:
	    case 32:
	    case 33:
	    case 36:
	    case 37:
	    case 60:
	    case 61:
	    case 64:
	    case 65:
	    case 74:
	    case 75:
	        setPiezo(NOTE_A0_SHARP);
	        return 0;
	    default:
	        myNoTone();
	        return 1;
	}
}
