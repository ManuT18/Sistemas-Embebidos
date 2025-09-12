/*
 LCD1602 Keypad Shield 1.0 Test Sketch - LiquidCrystal Library
 
 Este sketch demuestra el uso del LCD1602 Key Shield 1.0.
 Para ello se toman los pulsos de los botones mediante la entrada
 analógica AD0 y, mediante la librería LiquidCrystal de Arduino 1.0,
 se muestra en el display la tecla pulsada. La librería LiquidCrystal
 permite controlar cualquier display LCD compatible con el 
 controlador Hitachi HD44780.
 
 La configuración de la Librería se realiza en base al esquemático
 del shield.
 
 Este ejemplo está basado en un ejemplo provisto con la documentación del
 LCD1602 Keypad Shield, el cuál ha sido adaptado en base a los ejemplos 
 de la librería LiquidCrystal de Arduino.
 
 http://www.arduino.cc/en/Tutorial/LiquidCrystal
 http://arduino.cc/en/Reference/LiquidCrystal
 
 
 */
#include <stdbool.h>
#include <stdint.h>
#include "Arduino.h"
// include the library code:
#include <LiquidCrystal.h>

// these constants won't change.  But you can change the size of
// your LCD using them:
const uint8_t numRows = 2;
const uint8_t numCols = 16;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//Key message
char msgs[5][17] = 
{
    " Right Key:  OK ", 
    " Up Key:     OK ", 
    " Down Key:   OK ", 
    " Left Key:   OK ", 
    " Select Key: OK "
};
uint16_t adc_key_val[5] ={30, 150, 360, 535, 760 };
uint8_t NUM_KEYS = 5;
uint16_t adc_key_in;
uint16_t key=-1;
uint16_t oldkey=-1;


// Convert ADC value to key number
uint16_t get_key(unsigned int input)
{
    uint16_t k;
    for (k = 0; k < NUM_KEYS; k++)
        if (input < adc_key_val[k])
            return k;

    if (k >= NUM_KEYS)
        k = -1;     // No valid key pressed

    return k;
}


void setup() 
{
    pinMode(10, OUTPUT);
    analogWrite(10, 100);
    // set up the LCD's number of columns and rows: 
    lcd.begin(numCols,numRows);
    lcd.setCursor(0, 0);
    lcd.print("Backlight test");
    delay(2000);
    lcd.clear();
}

void loop()
{
    for (int i = 0; i < 510 ; i++) {
        analogWrite(10, i);
        delay(10);
    }
    lcd.setCursor(0, 1);      //line=1, x=0
    lcd.print("ciclando");
    delay(1000);
    lcd.setCursor(8, 1);      //line=1, x=8
    lcd.print(".");
    delay(500);
    lcd.setCursor(9, 1);      //line=1, x=9
    lcd.print(".");
    delay(500);
    lcd.setCursor(10, 1);      //line=1, x=10
    lcd.print(".");
    delay(500);
    lcd.clear();
}

//    0 -------- 256
//    257 ------ 511
//    512 ------ 767
//    768 ------ 1023
