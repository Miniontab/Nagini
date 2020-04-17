/*
 * This code initializes and rectifies the problem with all connected servo motors. 
 * The problem with the servo motors is that whenever they are connected to power, either through battery or arduino, 
 * they make an initial turn/rotation of approx. 45 deg.
 * 
 * This code rotates them back with same amount so that the direction of the motor is paralell to the rotational propeller-shaft.
 */

// Include the Servo library -------------------------------------------------------------
#include <Servo.h> 
#include <SM_iniCorrection.h>



// CREATING SERVO OBJECTS, DECLARING HOW MANY AND PLACING THEM IN AN ARRAY:---------------
// Create a servo object 
Servo Servo1; 
//Servo Servo2; 

// Declaring how many servos are connected
int N = 1;

// Create an array of all servo objects
Servo servoArray[] = {Servo1};



// DECLARING SERVO PINS AND PLACING THEM IN AN ARRAY:--------------------------------------
// Declare the Servo pin 
int servo1Pin = 53;
//int servo2Pin = 52

// Create an array of servo pins
int servoPinArray[] = {servo1Pin};

 

// DECLARING USUAL ROTATIONAL VALUES:------------------------------------------------------
int Origin_pos = 45;
int Left_pos = 140; // 50+90
int Right_pos = -40; // 50-90= -40 







void setup() { 
  // initializing all servo motors
  servoSetup(N, Origin_pos);


}
void loop(){ 
   
 
}
