/*
  SM_iniCorrection.h - Library for initializing and correcting the rotation trigger of servomotors when they're connected to a power source.
  Created by Singh, 17.04.2020 18:15

 
 * This code initializes and rectifies the problem with all connected servo motors.
 * The problem with the servo motors is that whenever they are connected to power, either through battery or arduino,
 * they make an initial turn/rotation of approx. 45 deg.
 *
 * This code rotates them back with same amount so that the direction of the motor is paralell to the rotational propeller-shaft.
 
*/
#ifndef SM_iniCorrection_h
#define SM_iniCorrection_h

#include "Arduino.h"
// Include the Servo library 
#include <Servo.h> 

void servoSetup(int N, int Origin_pos);
void iniServo(int angle, int i);


#endif