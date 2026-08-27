#include <Wire.h>
#include <Arduino.h>

// Declaración de la función callback
void requestEvent();

void setup() {
  Serial.begin(9600);
  Serial.println("Iniciando Arduino como esclavo I2C...");

  Wire.begin(0x08);              // Dirección I2C del esclavo
  Wire.onRequest(requestEvent);  // Ahora ya la conoce
}

void loop() {
  Serial.println("Arduino esperando solicitud I2C...");
  delay(2000);
}

// Definición de la función
void requestEvent() {
  Wire.write("Hola",18);   // Envía un carácter
  //Serial.println("Se envió 'H' al maestro I2C");
}

