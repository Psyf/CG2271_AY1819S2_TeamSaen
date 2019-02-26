#include <Arduino.h>
#include <avr/io.h>
#include "motorDriver.h"

#define LEFT_MOTORS_FORWARD 11
#define LEFT_MOTORS_BACKWARD 10
#define RIGHT_MOTORS_FORWARD 9
#define RIGHT_MOTORS_BACKWARD 6
#define SLOWDOWN 50

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

void moveForward(int speed) {
	if (speed > 255) speed = 255;
	else if (speed < 0) speed = 0;
	stop();
	analogWrite(LEFT_MOTORS_FORWARD, speed);
	analogWrite(RIGHT_MOTORS_FORWARD, speed);
}

void moveBackward(int speed) {
	if (speed > 255) speed = 255;
	else if (speed < 0) speed = 0;
	stop();
	analogWrite(LEFT_MOTORS_BACKWARD, speed);
	analogWrite(RIGHT_MOTORS_BACKWARD, speed);
}

void moveLeft(int speed) {
	if (speed > 255) speed = 255;
	else if (speed < 0) speed = 0;
	stop();
	analogWrite(LEFT_MOTORS_FORWARD, speed-SLOWDOWN);
	analogWrite(RIGHT_MOTORS_FORWARD, speed);
}

void moveRight(int speed) {
	if (speed > 255) speed = 255;
	else if (speed < 0) speed = 0;
	stop();
	analogWrite(LEFT_MOTORS_FORWARD, speed);
	analogWrite(RIGHT_MOTORS_FORWARD, speed-SLOWDOWN);
}
