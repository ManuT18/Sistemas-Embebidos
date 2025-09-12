
// ====================================
//                ATmega328p
//               
//              +--------+
//            --+ o  Vcc +------------
//  LED - PB5 --+        +--
//            --+        +--
//  ------------+ GND    +--
//              +--------+
// ====================================
// 
// Programa que cambia la frecuencia del led de la placa arduino dependiendo de un valor de entrada por la 
// entrada serie mediante un puntero a funcion

#include <Arduino.h>
void (*punteroafuncion)();



  void Blink05HZ ()
  {
   digitalWrite(13,HIGH);
   delay(1000);
   digitalWrite(13,LOW);
   delay(1000);
    
  }

  void Blink2HZ ()
  {
   digitalWrite(13,HIGH);
   delay(250);
   digitalWrite(13,LOW);
   delay(250);
    
  }



void Apagado ()
  {
   
   digitalWrite(13,LOW);
   
    
  }


  void cambiardefuncion(){
    uint8_t data = Serial.read();
    switch (data)
    {
      case '1':
        punteroafuncion=Blink05HZ;
        break;

      case '2':
        punteroafuncion=Blink2HZ;
        break;

      case '3':
        punteroafuncion=Apagado;
        break;
   
      }
      
    }


void setup()
{
  Serial.begin(9600);
  pinMode (13, OUTPUT);
  punteroafuncion = Blink05HZ;
  
 }

 void loop ()
 {
  punteroafuncion();

  if (Serial.available())
  {
    cambiardefuncion();
    }
  
  } 




