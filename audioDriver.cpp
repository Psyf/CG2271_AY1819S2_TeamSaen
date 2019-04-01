#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

#include <Arduino.h>
#include <avr/io.h>


#include "audioDriver.h"

const char BABYSHARKMAIN[BABYSHARKMAINLEN] = {NOTE_D0,NOTE_D0,NOTE_D0,NOTE_D0,NOTE_E0,NOTE_E0,NOTE_E0,NOTE_E0,NOTE_G0,NOTE_X,NOTE_X,NOTE_X,NOTE_G0,NOTE_X,NOTE_G0,NOTE_X,NOTE_X,NOTE_X,NOTE_G0,NOTE_X,NOTE_G0,NOTE_X,NOTE_X,NOTE_X,NOTE_G0,NOTE_X,NOTE_G0,NOTE_X,NOTE_X,NOTE_X};
const char BABYSHARKEND[BABYSHARKENDLEN] = {NOTE_D0,NOTE_D0,NOTE_D0,NOTE_D0,NOTE_E0,NOTE_E0,NOTE_E0,NOTE_E0,NOTE_F0,NOTE_X,NOTE_X,NOTE_X};


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
// plays baby shark at desired part of the song
// p ranges from 0 to 101 for a total 102 notes.
// returns TRUE if p > than song length (ie the end of the song is reached)
bool playBabyShark(unsigned int p) {
	if (p < 90) {
		setPiezo(BABYSHARKMAIN[p%30]);
	}
	else if (p < 102){
		setPiezo(BABYSHARKEND[p%30]);
	}
	else {
		return 1;
	}
	return 0;
}

void playInitSong() {
	return;
}

// make victory song using notes
void playVictorySong() {
	return;
}
