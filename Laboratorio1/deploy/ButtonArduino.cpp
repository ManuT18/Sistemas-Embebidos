/*
  Button
 http://www.arduino.cc/en/Tutorial/Button
 */
 
#include "Arduino.h"

// constantes que no cambian
// set pin numbers:
const int buttonPin = 2;     // setear el pin del pulsador
const int ledPin =  13;      // setear el pin del led


int buttonState = 0;         // variable que lee el estado del pulsador

void setup() {
  // ??
  pinMode(ledPin, OUTPUT);      
  // ??
  pinMode(buttonPin, INPUT);     
}

void loop(){
  // ??
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {     
    // turn LED on:    
    digitalWrite(ledPin, HIGH);  
  } 
  else {
    // turn LED off:
    digitalWrite(ledPin, LOW); 
  }
}
