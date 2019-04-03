#define NOTE_C0 119
#define NOTE_D0 106
#define NOTE_E0 94
#define NOTE_F0 90
#define NOTE_G0 79
#define NOTE_G0_SHARP 75
#define NOTE_A0 71
#define NOTE_A0_SHARP 67
#define NOTE_B0 63
#define NOTE_C1 59
#define NOTE_D1 53
#define NOTE_E1 47
#define NOTE_F1 45
#define NOTE_G1 40
#define NOTE_X 0
#define BABYSHARKMAINLEN 30
#define BABYSHARKENDLEN 12
#define TIMEPERNOTE 125
#define SPEAKE

#define BBYSHARK 0
#define WINSONG 1
#define CONNECTSONG 2

void setupAudio();
bool playBabyShark(unsigned int p);
bool playInitSong(unsigned int p);
bool playVictorySong(unsigned int p);
char statusToSong(char status);
void myNoTone();
