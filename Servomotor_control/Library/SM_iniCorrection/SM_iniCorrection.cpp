/*
    SM_iniCorrection.h - Library for initializing and correcting the rotation trigger of servomotors when they're connected to a power source.
  Created by Singh, 17.04.2020 18:15

  /*
 * This code initializes and rectifies the problem with all connected servo motors.
 * The problem with the servo motors is that whenever they are connected to power, either through battery or arduino,
 * they make an initial turn/rotation of approx. 45 deg.
 *
 * This code rotates them back with same amount so that the direction of the motor is paralell to the rotational propeller-shaft.
 
*/

#include "Arduino.h"
#include "Servo.h"
#include "SM_iniCorrection.h"
 

// External variables:
extern Servo servoArray[];
extern int servoPinArray[];
extern Servo Servo1;
extern int N;
extern int Origin_pos;




// Functions
void servoSetup(int N, int Origin_pos) {

    for (int i = 0; i <= N; i++) {
        iniServo(Origin_pos, i);
    }

}



void iniServo(int Origin_pos, int i)
{
    servoArray[i].attach(servoPinArray[i]);
    servoArray[i].write(Origin_pos);
    delay(1500);   
}
