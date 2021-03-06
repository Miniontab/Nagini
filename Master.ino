/* Author: Mehtab Singh
                                                                      PLEASE READ BEFORE PROCEEDING FOR CODE

   This master source file incorporates software to control Nagini v0.1.
   An accompanying ASMD chart explaining the behaviour of this software can be found at https://www.dropbox.com/s/i791sz6ga2d7ejx/ASMD.jpg?dl=0

   Few words about system-of-interest (more info found in referenced documents above):
   The system-of-interest is a robotic snake that consists of 4 servo motors (and a motor driver) for turning s-o-i,
   a DC motor (12VDC-300RPM) for forward propulsion (actuating a single wheel beneath snake's head),
   a LCD screen for displaying messages,
   an IR (infrared) sensor and accompanying remote for IR communication
   and a HC-SR04 sensor for measuring distance to an immediate object.
   This allows for a system that can follow an object (Autonomous mode) or to control the snake manually (type of motion is lateral undulation or serpentine movement )


   The structure of this software:
   1. Including libraries, defining/declaring or instantiating necessary variables or component objects,
   2. In setup: Initializing components and code as necessary
   3. In loop: Magic happens.

   The behaviour of this software:
   1. At initialization, the LCD screen will ask user to choose a operational mode for system-of-interest; that is either Autonomous or
      Manual (which is manual control of Nagini with serpentine motion). The user sets this with key press on the infrared remote; pressing the button with '1' on it will make the
      system-of-interest autonomous, while any other key press is manual control.

      A figure depicting the infrared(IR) remote control codes for used IR remote (CarMP3: http://www.geeetech.com/wiki/index.php/Arduino_IR_Remote_Control).
      Credits are due where credits are deserved (for the following figure): https://github.com/steakknife
         ------------------
       /                    \
      |  CH-    CH     CH+   |
      | FFA25D FF629D FFE21D |
      |                      |
      |  |<<     >>|   |>||  |
      | FF22DD FF02FD FFC23D |
      |                      |
      |   -       +     EQ   |
      | FFE01F FFA857 FF906F |
      |                      |
      |   0      100+  200+  |
      | FF6897 FF9867 FFB04F |
      |                      |
      |   1       2     3    |
      | FF30CF FF18E7 FF7A85 |
      |                      |
      |   4       5     6    |
      | FF10EF FF38C7 FF5AA5 |
      |                      |
      |   7       8     9    |
      | FF42BD FF4AB5 FF52AD |
      |                      |
      |         Car          |
      |         mp3          |
       \                    /
         ------------------
      (FFFFFFFF for repeat when a button is held)


   2. In autonomous mode: See the accompanying ASMD chart noted on line 3.
     In "Auto" state, control path determines if there is an input signal corresponding to "exit"; 
     if so, the control path clears the LCD subsystem and transitions back to the "Bootup" state.
     Otherwise, the control path continues to check whether there is lower than 10cm distance to an object; 
     if true, system-of-interest moves backwards in order to increase the distance to object and checks the Mealy statement again. 
     If the Mealy statement returns false, the control path continues to next Mealy statement in order to check whether the distance is greater than or equal to 10cm. 
     In case that is true, the system-of-interest moves towards the object to decrease the distance and checks the Mealy statement again. 
     Otherwise the control path executes Moore statement, i.e. it turns the head in order to survey its surroundings for objects. Thereafter, it loops back to same state again to repeat flow.


   3. In manual mode:
      The state box is empty so control path continues to first Mealy statement; checking whether a infrared signal for state-exit has been received.
      If not, control path continues towards second Mealy statement. In this statement, the control path determines whether a signal for moving forward has been received.
      If so, the system-of-interest moves forwards. Otherwise, the control path continues to check whether different input signals have been received, and in case they have, the corresponding action is executed.
      As seen in the ASMD chart, this continues until the control path reaches the last Mealy statement in "Manual\_serpentine" state.
      In case a signal for turning left is received, the system-of-interest will turn left, otherwise the control path loops back to the beginning of same state.
*/

// Including libraries:
#include <IRremote.h>                         // Library for the infrared communication with IR remote.
#include <Servo.h>                           // Library for servo motors.
#include <SM_iniCorrection.h>               // Library for correcting the servo motor at initialization. 
// The rationale for an initial correction of the servo motors is:
// Whenever they are connected to power,
// either through battery or arduino,
// they make an initial turn/rotation of approx. 5-10 deg.

#include <Wire.h>                           // Library for I2C (LCD in our case) communication. 
#include <LiquidCrystal_I2C.h>              // Library for I2C LCD functions (similar to LiquidCrystal library)



//-------------- IR related preamble --------------------------------
// Defining the infrared receiving pin on Arduino(the datapin),
// instatiating a receiver at that pin and a variable that holds the value of what is received:

#define IR_DataPin 19                       // Infrared receiving pin on Arduino is defined
IRrecv IR_Receiver(IR_DataPin);             // An infrared receiver has now been tied to stated receiving pin
decode_results IR_Results;                  // A variable that holds the value of what has been received via IR communication.

// Declaring a constant IR value that will be used as the "exit" code depicted in first Mealy statements of "Auto" and "Manual_serpentine" states in ASMD chart.
// The value of this is FF52AD <--> key '9' on remote:

long int IR_Exit = 0xFF52AD;                // A variable that holds an "exit"-hex value (key 9) for going back to BootUp State
long int IR_Forward = 0xFF18E7;            // A variable that holds an "move forward"-hex value (key 2)(for manual state)
long int IR_Backward = 0xFF4AB5;           // A variable that holds an "move backward"-hex value (key 8)(for manual state)
long int IR_Right = 0xFF5AA5;             // A variable that holds an "move right"-hex value (key 6)(for manual state)
long int IR_Left = 0xFF10EF;              // A variable that holds an "move left"-hex value (key 4)(for manual state)


//-------------- Servo motor related preamble -----------------------
// Creating servo objects, declaring how many and placing them in an array:
Servo Servo1;
//Servo Servo2;
//Servo Servo3;
//Servo Servo4;

// Declaring how many servos are connected
const int N = 1;

// Create an array of all servo objects
Servo servoArray[N] = {Servo1};


// Declaring servo pins and putting them in an array
int servo1Pin = 53;   //Head                 
//int servo2Pin = 51;   //2nd link
//int servo3Pin = 49;   //3rd link
//int servo4Pin = 47;   //4th link

int servoPinarray[N] = {servo1Pin};  // Create an array of servo pins



// Declaring usual rotational values:
int Origin_pos = 90;                       // Original position of servo motors
int Right_pos = 15 ;                      // 90-75 gives a 90 deg turn, link will be turning right
int Left_pos = 165 ;                     // 90+75 gives a 90 deg turn , link will be turning right
int i = Origin_pos;                     // Global variable to turn the head and remember position


//-------------- DC Motor related preamble --------------------------
// Defining input- and enable pins:
#define EN 5  // white cable (used for speed control by adjusting PWM signal to motor)
#define IN2 7 // orange cable (-)
#define IN1 6 // yellow cable (+)


//-------------- LCD related preamble -------------------------------

// Instantiating a LCD screen and setting its address in one line:

LiquidCrystal_I2C LCD_Screen(0x27, 2, 1, 0, 4, 5, 6, 7);         // Format: (I2C_ADDR,EN_PIN,RW_PIN,RS_PIN,D4_PIN,D5_PIN,D6_PIN,D7_PIN)
// To find the address, see https://create.arduino.cc/projecthub/abdularbi17/how-to-scan-i2c-address-in-arduino-eaadda



//-------------- Distance measuring sensor (HC-SR04) preamble ------- // For more info, see https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf

// Defining pins for trigger (TRIG) and echo (ECHO):

#define tPin 9 // hvit
#define ePin 10 //gul

// Declaring variables for duration (how long it takes for a transmitted ultrasonic wave to be received)
// and distance (how far it is to an object)

unsigned long t;                                                // Variable for the time a wave reflection registers.
// This is calculated by pulseIn(Eyes_echoPin, HIGH) function.
// Keep in mind that this calculates the time it takes for the wave to travel 2x the distance (forth and back)
// Unit: ??s (microseconds)

int dist;                                                       // Variable for the distance(cm) to an object.
// The distance to an object is calculated by a manipulation of "Speed [m/s] = Distance [m] / Time [s]".
// In our case, the time given by pulseIn is for 2x distance traveled, which gives us
// --> 2x dist [m] = Speed [m/s] * t [s] <--> dist [m] = (Speed [m/s] * t [s]) / 2

// The speed of ultrasonic waves in air is approx. 340 m/s ~ 0.034 cm/??s (see https://www.ndk.com/en/sensor/ultrasonic/basic01.html)
// This gives a rationale for
// dist [cm] = (0.034 [cm/??s] * t [??s]) / 2;






//-------------- State Machine related preamble ---------------------
// Declaring States of the State Machine. Our states, as depicted in ASMD chart, are {BootUp, Auto, Manual}.
// We will also need to declare to variables that can hold one of these states at a time:

enum State {BootUp, Auto, Manual};            // States declared by using enum (enumerated type)
State presentState, nextState;                // A presentState and nextState variable of type "State" is used to hold what state we are in
// and which state to transition to next.



void setup() {

  //Starting serial communication (UART, baudrate is 9600)):
  Serial.begin(9600);


  //------------ State Machine related setup -----------------------
  // Initial State is Bootup:
  presentState = BootUp;
  nextState = presentState;  // Default back to same state



  //------------ IR related setup ----------------------------------
  // Initializing IR receiver for receiving process:
  IR_Receiver.enableIRIn();

  // Making the on-board LED on Arduino Mega 2560 blink during TX & RX (that is, during transmission):
  IR_Receiver.blink13(true);

  // Resetting the IR receiving value:
  IR_Results.value = 0;

  // Defining an interrupt so that whenever IR receiver gets new exit signal from remote, it is handled:
  attachInterrupt(digitalPinToInterrupt(IR_DataPin), getIRval, CHANGE);

  //------------ Servo motor related setup -------------------------
  // Attaching all servos:
  Servo1.attach(servo1Pin);
  Servo1.write(Origin_pos);
//  Servo2.attach(servo2Pin);
//  Servo3.attach(servo3Pin);
  //Servo4.attach(servo4Pin);

  // Initial correction of Servo motors:
  // All connected servo motors are turned to their original position (facing straight)
//  servoSetup(N, Left_pos);            // Then they're turned so that the link is facing left.
//  delay(1000);
//  servoSetup(N, Right_pos);          // When we turn them back to the original position now, they're accurately positioned.
//  delay(1000);
//  servoSetup(N, Origin_pos);
Servo1.write(Origin_pos);

  //----------- DC motor related setup -----------------------------
  // Setting input- and enable pins as output:
  pinMode(EN, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);



  //------------ LCD related setup ---------------------------------
  // Start the LCD screen, turning the backlight on for brightness (visibility to read), and writing:
  LCD_Screen.begin (16, 2); // 16 x 2 LCD module that we are using
  LCD_Screen.setBacklightPin(3, POSITIVE); // BL, BL_POL
  LCD_Screen.setBacklight(HIGH);

  // Display a nice welcome message for the user :-) (5 seconds) :
  LCD_Screen.setCursor(5, 0);         // Sets the cursor at 3rd column, 0th row to display "Hello :-)" in center
  LCD_Screen.print("Hello!");         // Display message
  LCD_Screen.setCursor(2, 1);         // Sets the cursor at 3rd column, bottom row to display message centered
  LCD_Screen.print("I'm Nagini :)");  // Display message
  delay(5000);                        // Hold it for 5 seconds
  LCD_Screen.clear();                 // Clear screen




  //------------ Distance-measuring sensor (HC-SR04) setup ---------
  // Setting the pinMode for the trigger to be an output, echo to be an input:
  pinMode(tPin, OUTPUT);
  pinMode(ePin, INPUT);








}

void loop() {
  // put your main code here, to run repeatedly:
  StateMachine();




}

void StateMachine() {

  switch (presentState) {
    case BootUp:
      BU();
      break;

    case Auto:
      Serial.println("N?? er jeg i start av A");
      ExitCheck();      
      A_Mealy2();
      
      break;

    case Manual:
      //Serial.println("N?? er jeg i manual");
  Serial.println("N?? er jeg i start av M");  
  ExitCheck();
  
  
  if(IR_Receiver.decode(&IR_Results)){
  switch (IR_Results.value) {
      case 0xFF18E7:                               // If the IR value is forward:
        // Move forwards:
        analogWrite(EN, 255);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN1, LOW);
    
        Serial.println("Moving forward");
        IR_Receiver.resume();                  // Resume IR reception

        break;

      case 0xFF4AB5:                          // If the IR value is backward_
        // Move backwards:
        analogWrite(EN, 100);
        digitalWrite(IN2, LOW);
        digitalWrite(IN1, HIGH);
    
        Serial.println("Moving backward");
        IR_Receiver.resume();                  // Resume IR reception

        break;

        case 0xFF5AA5:                          // If the IR value is turn right
         // Turn head right:
        for (i; i >= Right_pos + 25; i = i - 10) {
          Servo1.write(i);
        }
        // Move forwards:
        analogWrite(EN, 255);
        digitalWrite(IN2, LOW);
        digitalWrite(IN1, HIGH);
    
        IR_Receiver.resume();                  // Resume IR reception

        break;

        case 0xFF10EF:                          // If the IR value is turn left
         // Turn head left:
        for (i; i <= Left_pos + 25; i = i + 10) {
          Servo1.write(i);
        }
        // Move forwards:
        analogWrite(EN, 255);
        digitalWrite(IN2, LOW);
        digitalWrite(IN1, HIGH);
    
        IR_Receiver.resume();                  // Resume IR reception

        break;

        default:
        nextState = Manual;           // Start Manual again
        presentState = nextState;   // Update state

        break;


    }
  }

  break;

 
      
  }
}

void BU() {
  Serial.println("N?? er jeg i Bootup");


  // State box: Moore statement: Display following message on LCD: "Choose operational mode" on first row, "l(AUTO)/0(MAN)" on second row:
  LCD_Screen.home();                                             // Setting the cursor to 0,0
  LCD_Screen.print("Choose operational mode:");                  // Printing first message on top row

  LCD_Screen.setCursor(0, 2);                                    // Position cursor at 0th column, 2nd row
  LCD_Screen.print("l(AUTO) / 0(MAN)");                            // Printing second message on bottom row

  // First Mealy statement: Keep checking which state to transition to while there's no IR transmission. A 1 is Auto, 0 is Manual. 1 is IR code 0xFF30CF, 0 is IR code 0xFF6897
  // Clearing the variable for IR values first, so the following code doesn't use old values:
  //IR_Results = NULL;
  //IR_Receiver.resume();


  if (IR_Receiver.decode(&IR_Results)) {             // This will return TRUE if there's any results from IR transmission

    //Serial.println(IR_Results.value, HEX);        // For testing purposes: See the value in serial monitor

    switch (IR_Results.value) {
      case 0xFF30CF:                               // If the IR value is key '1' then the following will happen:
        LCD_Screen.clear();                       // Clear the LCD screen before displaying new message
        LCD_Screen.home();                       // Set cursor at 0,0
        LCD_Screen.print("Autonomous mode");    // Print "Autonomous mode selected" for 5 seconds
        LCD_Screen.setCursor(0, 2);            // Position cursor at 0th column, 2nd row to finish message
        LCD_Screen.print("selected");


        delay(4000);                           // Hold message for 4 seconds
        LCD_Screen.clear();                    // Clear screen

        IR_Results.value = 0;                // Reset the IR value

        IR_Receiver.resume();                  // Resume IR reception


        nextState = Auto;                     // Set the state to transition to (Auto in this case)

        break;

      case 0xFF6897:                          // If the IR value is key '0' then the following will happen:
        LCD_Screen.clear();                   // Clear the LCD screen before displaying new message
        LCD_Screen.home();                    // Set cursor at 0,0
        LCD_Screen.print("Manual mode");      // Print "Manual mode selected" for 5 seconds
        LCD_Screen.setCursor(0, 2);          // Position cursor at 0th column, 2nd row to finish message
        LCD_Screen.print("selected");
        delay(5000);                         // Hold message for 5 seconds
        LCD_Screen.clear();                  // Clear screen

        IR_Receiver.resume();               // Resume IR reception

        IR_Results.value = 0;             // Reset the IR value


        nextState = Manual;                 // Set the state to transition to (Manual in this case)

        break;


        //          default:                         // If any other key is pressed, tell user to press 1 or 0
        //            LCD_Screen.clear();            // Clear the LCD screen before displaying new message
        //            LCD_Screen.setCursor(0,0);     // Set cursor at 0,0
        //            LCD_Screen.print("Please choose:");
        //            LCD_Screen.setCursor(0,1);
        //            LCD_Screen.print("1(A) or 0(M)");
        //
        //            LCD_Screen.setCursor(15,1);      // A timer to count down 5 seconds so the user can choose again
        //            LCD_Screen.print("5");
        //            delay(1000);
        //            LCD_Screen.setCursor(15,1);
        //            LCD_Screen.print("4");
        //            delay(1000);
        //            LCD_Screen.setCursor(15,1);
        //            LCD_Screen.print("3");
        //            delay(1000);
        //            LCD_Screen.setCursor(15,1);
        //            LCD_Screen.print("2");
        //            delay(1000);
        //            LCD_Screen.setCursor(15,1);
        //            LCD_Screen.print("1");
        //            delay(1000);
        //            LCD_Screen.setCursor(15,1);
        //            LCD_Screen.print("0");
        //            //IR_Results.value = 0;
        //
        //            break;

    }

    IR_Receiver.resume();  // Resume the IR reception process

  }

  presentState = nextState;  // Update the current state.

}

void A() {
 
}





void A_Scan() {
  if (i == Origin_pos) {  // If statement to check which direction to turn.
    // If head is already positioned left, then it will turn right.
    // If it is already positioned right, then it will turn left.
    // If it is in original position, then it will turn right.
    // Turning the head slowly from original position, to right:
    for (i; i >= Right_pos + 25; i = i - 10) {
      Servo1.write(i);
    }

    Serial.print("i er "); Serial.println(i);

  } else if (Origin_pos < i <= Left_pos) {        // if the position of the head is left

    // Turning the head slowly from left, to right:
    for (i; i >= Right_pos + 25; i = i - 10) {  // Turning the head from left to right
      Servo1.write(i);
    }

    Serial.print("i er "); Serial.println(i);
    
  } else if (Origin_pos > i >= Right_pos) { // if the position of the head is right
    for (i; i <= Left_pos + 25; i = i + 10) {  // Turning head from right to left
      Servo1.write(i);
    }

    Serial.print("i er "); Serial.println(i);

  }

}

void DistPrint() {
  LCD_Screen.setCursor(0, 0); // Sets the location at which subsequent text written to the LCD_Screen will be displayed
  LCD_Screen.print("Distance: "); // Prints string "Distance" on the LCD_Screen
  LCD_Screen.setCursor(10, 0);
  LCD_Screen.print("   ");
  LCD_Screen.setCursor(10, 0);
  LCD_Screen.print(dist); // Prints the distance value from the sensor
  //LCD_Screen.clear();
  //  LCD_Screen.print("cm");
  delay(250);
}

void DistCheck() {
  digitalWrite(tPin, LOW);
  delayMicroseconds(2);

  digitalWrite(tPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(tPin, LOW);

  t = pulseIn(ePin, HIGH);

  dist = t*0.034/2;

}

void A_Mealy2() {

Serial.println("N?? er jeg i mealy2");
  
Second_Mealy:
  DistCheck();
  DistPrint();

  if (dist < 10 && dist !=0) {
    // Move backwards:
    analogWrite(EN, 155);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN1, LOW);

    goto Second_Mealy;
  } else {
    //    DistCheck();
    //    DistPrint();
    A_Mealy3();
  }
}

void A_Mealy3() {

Serial.println("N?? er jeg i mealy3");
  
Third_Mealy:
  DistCheck();
  DistPrint();

  if (dist >= 10) {

    // Move forwards:
    analogWrite(EN, 255);
    digitalWrite(IN2, LOW);
    digitalWrite(IN1, HIGH);

    goto Third_Mealy;

  } else {
    //    DistCheck();
    //    DistPrint();
    A_Scan();
    nextState = Auto;           // Start Auto again
    presentState = nextState;   // Update state
  }
}

void M() {

  


}






void ExitCheck() {

  Serial.println("N?? er jeg i exitcheck");
  Serial.println(IR_Results.value, HEX);
  
  // Check if the IR signal value is equal to the exit value:
  if (IR_Results.value == IR_Exit) {   // In case it is a match, then the LCD_Screen is cleared, cursor is set to 0,0,
    // and a message saying "exiting" is displayed for 2 seconds.
    LCD_Screen.clear();
    LCD_Screen.home();
    LCD_Screen.print("Exiting...");
    IR_Receiver.resume();                  // Resume IR reception

    Serial.println("n?? var koden exit");

    // Correction of Servo motors:
    // All connected servo motors are turned to their original position (facing straight)
//    servoSetup(N, Left_pos);               // Then they're turned so that the link is facing left.
//    delay(1000);
//    servoSetup(N, Right_pos);            // When we turn them back to the original position now, they're accurately positioned.
//    delay(1000);
    servoSetup(N, Origin_pos);

    i = Origin_pos;                       // Update head position variable i: Position of head is now straight ahead (original position)

    //delay(2000);                         // Give the motors 2 seconds to adjust

    nextState = BootUp;                // Transition back to Bootup if exit key is pressed

    presentState = nextState;

  }


  if(presentState == BootUp){
        StateMachine();
      }
  
 

}

void getIRval() {

  if (IR_Receiver.decode(&IR_Results)) {
    delay(200);
    Serial.println(IR_Results.value, HEX);


    IR_Receiver.resume();
  }
}
