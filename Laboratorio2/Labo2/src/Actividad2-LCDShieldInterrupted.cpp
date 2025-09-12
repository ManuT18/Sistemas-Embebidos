#include <stdbool.h>
#include <stdint.h>
#include <Arduino.h>
#include <LiquidCrystal.h>
#include "fnqueue.h"
#include "critical.h"
#include "Actividad2-Driver.h"

// Initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Key message
// char msgs[5][17] = 
// {
//     " Right Key:  OK ", 
//     " Up Key:     OK ", 
//     " Down Key:   OK ", 
//     " Left Key:   OK ", 
//     " Select Key: OK "
// };

void onKeyDown(int tecla);
void onKeyUp(int tecla);

void setup() 
{
    lcd.begin(16,2);
    analogWrite(10, 100); // Controla intensidad backlight
    lcd.print("ADC con ISR");
    delay(1000);
    lcd.clear();

    fnqueue_init();
    ADC_init();
    sei();  // habilitar interrupciones globales

    key_down_callback(onKeyDown);
    key_up_callback(onKeyUp);
}

void loop()
{
    // Ejecuta funciones encoladas
    fnqueue_run();

    for (int i = 0; i < 255 ; i++) {
        analogWrite(10, i);
        delay(10);
    }
    lcd.setCursor(0, 1);      //line=1, x=0
    lcd.print("ciclando");
    delay(300);
    lcd.setCursor(8, 1);      //line=1, x=8
    lcd.print(".");
    delay(300);
    lcd.setCursor(9, 1);      //line=1, x=9
    lcd.print(".");
    delay(300);
    lcd.setCursor(10, 1);      //line=1, x=10
    lcd.print(".");
    delay(300);
    lcd.clear();
}

void onKeyDown(int tecla) {
    lcd.setCursor(0, 1);
    lcd.print("Presionada: ");
    lcd.print(tecla);
}

void onKeyUp(int tecla) {
    lcd.setCursor(0, 1);
    lcd.print("Liberada: ");
    lcd.print(tecla);
}

