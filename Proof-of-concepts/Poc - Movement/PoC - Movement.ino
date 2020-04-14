/*  
 *   PROOF-OF-CONCEPT: NAGINI 0.1v STRAIGHT-LINE MOVEMENT
 *   
 *   SUPERVISORS WANT TO SEE PROOF-OF-CONCEPT OF MOVEMENT IN WEEKLY MEETING HELD 14.04.2020 12:00.
 *   
*/

//Defining input- and enable pins:
#define EN 5  // green cable (used for speed control by adjusting PWM signal to motor)
#define IN2 6 // brown cable (-)
#define IN1 7 // yellow cable (+)


void setup() {
  // Setting input- and enable pins as output from Arduino:
  pinMode(EN, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
    
}




void loop() {
  // Nagini 0.1v will only travel for 2s; motor off (1s) - motor on (2s)- motor off (1s) - ...

  // Send LOW PWM signal to EN for 0 speed: 
  analogWrite(EN, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  

  delay(1000);

  // Send HIGH PWM signal to EN for max speed:
  // Setting motor direction forward; IN1 = HIGH && IN2 = LOW
  analogWrite(EN, 100);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  

  delay(2000);
    
}
