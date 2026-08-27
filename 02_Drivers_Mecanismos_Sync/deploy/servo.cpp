#include <Servo.h>
#include <Arduino.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

void setup() {
     Serial.begin(9600);
     myservo.attach(9);  // attaches the servo on pin 9 to the servo object
}

void loop() {
//   for (position = 0; position <= 180; position++) { // goes from 0 degrees to 180 degrees
//     // in steps of 1 degree
//     myservo.write(position);              // tell servo to go to position in variable 'pos'
//     Serial.print("Posicion: ");
//     Serial.println(position);
//     delay(200);                       // waits 15ms for the servo to reach the position
//   }
//   for (position = 180; position >= 0; position--) { // goes from 180 degrees to 0 degrees
//     myservo.write(position);              // tell servo to go to position in variable 'pos'
//     Serial.print("Posicion: ");
//     Serial.println(position);
//     delay(200);                       // waits 15ms for the servo to reach the position
//   }
     int val = analogRead(A0);            // read the input on analog pin 0
     val = map(val, 0, 1023, 0, 180); // scale it to use it with the servo (value between 0 and 180)
     Serial.print("Valor leido: ");
     Serial.println(val);
     myservo.write(val);              // sets the servo position according to the scaled value
     delay(400);                       // waits for the servo to get there
}