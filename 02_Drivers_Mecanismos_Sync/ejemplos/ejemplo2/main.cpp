// ====================================
//                ATmega328p
//               
//              +--------+
//            --+ o  Vcc +------------
//            --+        +--
//            --+        +--A1    <-- Patilla de salida de potenciometro a la entrada A1 de la placa arduino
//  ------------+ GND    +--
//              +--------+
//
// Mediante el monitor serie podremos ir viendo que valores vamos obteniendo en el pin analógico A1
// según vayamos modificando la posición del potenciómetro con interrupciones
// ====================================


#include <avr/interrupt.h>
#include <Arduino.h>
volatile int readFlag;
const byte adcPin = 1;

// Value to store analog result
volatile int analogVal;

// Initialization
void setup(){

  Serial.begin(9600);
  ADMUX = bit(REFS0) | adcPin;  //01000000 | 00000001 = 01000001 = 0x41
  ADCSRA |= (1<<ADEN);                            //Power up the ADC
  ADCSRA |= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));   //Prescaler at 128 so we have an 125Khz clock source
  ADMUX  |= (1<<REFS0); 


  ADCSRA |= (1 << ADIE);      // Enable the interrupt
  ADCSRA |= (1 << ADATE);     // Set ADATE for free-running
  
 
  readFlag = 0;
  ADCSRA |= (1<<ADSC);        //Start a conversion
  sei();
  
}


// Processor loop
void loop(){

  // Check to see if the value has been updated
  if (readFlag == 1){
       // Perform whatever updating needed
     readFlag = 0;
  }
 
   //Serial.println(analogVal);            //print digital value to serial monitor
   float voltaje = (analogVal*5.0)/1023.0;
   // envia el valor del ADC por el puerto serie.
   Serial.print("Valor ADC: ");
   Serial.print(analogVal);
   //Serial.print('            ');
   Serial.print("\tVoltaje: ");
   Serial.println(voltaje);
   delay(500);
}


// Interrupt service routine for the ADC completion
ISR(ADC_vect){
  // Done reading
  readFlag = 1;
  // Must read low first
  analogVal = ADCL | (ADCH << 8);
}
