#include <Arduino.h>
#include <avr/io.h>
#include "motorDriver.h"


//LEFT WHEEL | PORT B
#define LEFT_MOTORS_FORWARD PB0  //Arduino pin9 //to be PB0 -pin 8
#define LEFT_MOTORS_BACKWARD PB1 //Arduino pin10 //to be PB1 -pin 9

//RIGHT WHEEL | PORT D
#define RIGHT_MOTORS_FORWARD PD2  //Arduino pin5 //to be PB2 -pin 10
#define RIGHT_MOTORS_BACKWARD PD3 //Arduino pin6 //to be PB3 -pin 11

uint8_t LeftMotorPortBitSelectorPattern = (1<<LEFT_MOTORS_FORWARD); //defaultds
uint8_t RightMotorPortBitSelectorPattern= (1<<RIGHT_MOTORS_FORWARD); //defaults

ISR(TIMER2_COMPA_vect){
  //controls Left MotorPower, Clears appropriate bits (PB0&PB1)
  PORTB &= ~(1<<PB0 | 1<< PB1);
}
ISR(TIMER2_COMPB_vect){
  //controls Right MotorPower, Clears appriopriate bits (PB2&PB3)
  PORTB &= ~(1<<PB2 | 1<< PB3);
}
ISR(TIMER2_OVF_vect){
  //turns on the pins at Whatever LeftMotorPinToPower and RightMotorPinToPower pins are. Forward and backward for each pin must be on the same PORT
  PORTB |= LeftMotorPortBitSelectorPattern | RightMotorPortBitSelectorPattern;
}

void setupMotors(){
  //clearing interrupt flag so that Motors can initialise properly
  noInterrupts();

  //SET PIN8 PIN9 PIN10 PIN11 to output
  DDRB |= (1<<DD0 |1<<DD1 | 1<<DD2 |1<<DD3);


  //setup TCCR2A - NORMAL MODE OC2A and OC2B disconnected
  TCCR2A &= ~( 1<<WGM00 | 1<<WGM01 | 1<<COM0A0 | 1<<COM0A1 | 1<<COM0B0 | 1<<COM0B1 );

  //setup TCCR2B - PRESCALAR SELECTION
  TCCR2B &= ~(1<<FOC0A | 1<<FOC0B | 1<<WGM02 | 1<<CS01 | 1<<CS00);
  TCCR2B |= (1<<CS02);


  //DISABLES TIMER INTTERUPTS (For now)
  TIMSK2 &= ~(1<<OCIE0B | 1<<OCIE0A | 1<<TOIE0);

  //initialse both Compare registers to 0. Just for completeness sake
  OCR2A = (uint8_t) 0;
  OCR2B = (uint8_t) 0;

  //setting our Motor pins to LOW
  PORTB &= ~(1<<PB0 | 1<<PB1 | 1<< PB2 | 1<<PB3);

  //begin intterupts
  interrupts();
}

void stop(){
  //DISABLES TIMER INTTERUPTS (For now)
  TIMSK2 &= ~(1<<OCIE2B | 1<<OCIE2A | 1<<TOIE2);
  //setting our Motor pins to LOW
  PORTB &= ~(1<<PB0 | 1<<PB1 | 1<< PB2 | 1<<PB3);
}


void setpower(unsigned int leftforward,unsigned int leftbackward, unsigned int rightforward, unsigned int rightbackward){
  if ( ((leftforward > 1  && leftbackward > 1) || (rightforward > 1  && rightbackward > 1)) || (leftforward < 1  || leftbackward < 1 || rightforward < 1  && rightbackward < 1)){
    //Guard clause. In case weird things happen
    //fail silently and ignore the command. its okay, RTOS will send proper termination/stop commands if connection is lost or no bluetooth message is received after a timeout period
    return;
  }

  if (leftforward==1 && leftbackward==1 && rightforward==1 && rightbackward==1){
    stop();
    return;
  }

  if (leftforward>1){ //implicitly leftbackward <= 1
    LeftMotorPortBitSelectorPattern = (1<<LEFT_MOTORS_FORWARD);
    OCR2A =  (uint8_t) leftforward;
  }
  else {
    LeftMotorPortBitSelectorPattern = (1<<LEFT_MOTORS_BACKWARD);
    OCR2A = (uint8_t) leftbackward;
  }

  if (rightforward>1){ //implicitly rightbackward <= 1
    RightMotorPortBitSelectorPattern = (1<<RIGHT_MOTORS_FORWARD);
    OCR2B =  (uint8_t) rightforward;
  }
  else {
    RightMotorPortBitSelectorPattern = (1<<RIGHT_MOTORS_BACKWARD);
    OCR2B = (uint8_t) rightbackward;
  }

  //ENABLE TIMER INTTERUPTS (For now)
  TIMSK2 |= (1<<OCIE0B | 1<<OCIE0A | 1<<TOIE0);
}

