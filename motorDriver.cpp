#include <Arduino.h>
#include <avr/io.h>
#include "motorDriver.h"

#define LEFT_MOTORS_FORWARD 5
#define LEFT_MOTORS_BACKWARD 10
#define RIGHT_MOTORS_FORWARD 6
#define RIGHT_MOTORS_BACKWARD 9

void setupMotors() {
	pinMode(LEFT_MOTORS_FORWARD, OUTPUT);
	pinMode(LEFT_MOTORS_BACKWARD, OUTPUT);
	pinMode(RIGHT_MOTORS_FORWARD, OUTPUT);
	pinMode(RIGHT_MOTORS_BACKWARD, OUTPUT);
}

// The following functions are for movement
// speed is the pwm frequency (0 to 255 for the 328p)
void stop() {
	analogWrite(LEFT_MOTORS_FORWARD, LOW);
	analogWrite(LEFT_MOTORS_BACKWARD, LOW);
	analogWrite(RIGHT_MOTORS_FORWARD, LOW);
	analogWrite(RIGHT_MOTORS_BACKWARD, LOW);
}

void setpower(unsigned int leftforward,unsigned int leftbackward, unsigned int rightforward, unsigned int rightbackward){
	analogWrite(LEFT_MOTORS_FORWARD, leftforward);
	analogWrite(LEFT_MOTORS_BACKWARD, leftbackward);
	analogWrite(RIGHT_MOTORS_FORWARD, rightforward);
	analogWrite(RIGHT_MOTORS_BACKWARD, rightbackward);
}

