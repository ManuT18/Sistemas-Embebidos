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
// según vayamos modificando la posición del potenciómetro sin interrupciones
// ====================================
#include <Arduino.h>



const byte adcPin = 1;  // A1
  
bool working;
 
void setup ()
  {
  Serial.begin (9600);
  
 //ADCSRA =  bit (ADEN);   // turn ADC on
 //ADCSRA |= bit (ADPS0) |  bit (ADPS1) | bit (ADPS2);  // Prescaler of 128
 //ADMUX =   bit (REFS0) | (adcPin & 0x07);  // AVcc   

  ADMUX  = bit(REFS0) | adcPin;  //01000000 | 00000001 = 01000001 = 0x41
  ADCSRA |= (1<<ADEN);                            //Power up the ADC
  ADCSRA |= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));   //Prescaler at 128 so we have an 125Khz clock source
  ADMUX  |= (1<<REFS0); 

 // This is neccesary when you use INTERRUPT
 // ADCSRA |= (1<<ADIE); // Set ADIE in ADCSRA (0x7A) to enable the ADC interrupt.
 // ADCSRA |= (1<<ADSC); //Start converting for(;;
 
  
  }  // end of setup

void loop () 
  { 
  if (!working)
    {
    ADCSRA |= (1<<ADSC); //Start a conversion
    working = true;
    }
    
  // the ADC clears the bit when done
  if (bit_is_clear(ADCSRA, ADSC))
    {
    int value = ADC;  // read result
    working = false;
    float voltaje = (value*5.0)/1023.0;
    // envia el valor del ADC por el puerto serie.
    Serial.print("Valor ADC: ");
    Serial.print(value);
    //Serial.print('            ');
    Serial.print("\tVoltaje: ");
    Serial.println(voltaje);
    delay(500);
    
    }
    
  }  // end of loop  
