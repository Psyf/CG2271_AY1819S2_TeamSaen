#include <Arduino.h>
#include <avr/io.h>
#include "audioDriver.h"

const int BABYSHARKMAIN[BABYSHARKMAINLEN] = {NOTE_D0,NOTE_D0,NOTE_D0,NOTE_D0,NOTE_E0,NOTE_E0,NOTE_E0,NOTE_E0,NOTE_G0,NOTE_X,NOTE_X,NOTE_X,NOTE_G0,NOTE_X,NOTE_G0,NOTE_X,NOTE_X,NOTE_X,NOTE_G0,NOTE_X,NOTE_G0,NOTE_X,NOTE_X,NOTE_X,NOTE_G0,NOTE_X,NOTE_G0,NOTE_X,NOTE_X,NOTE_X};
const int BABYSHARKEND[BABYSHARKENDLEN] ={NOTE_D0,NOTE_D0,NOTE_D0,NOTE_D0,NOTE_E0,NOTE_E0,NOTE_E0,NOTE_E0,NOTE_F0,NOTE_X,NOTE_X,NOTE_X};

// import the notes as freq from the internet
void setupAudio() {
	pinMode(SPEAKER_PIN, OUTPUT);
}

void setPiezo(unsigned int freq){
	if (freq == 0){
		noTone(SPEAKER_PIN);
	}
	else{
		tone(SPEAKER_PIN,freq);
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

// make victory song using notes
void playVictorySong() {
	return;
}
