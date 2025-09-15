#include <stdbool.h>
#include <stdint.h>
#include <Arduino.h>
#include <LiquidCrystal.h>
#include "fnqueue.h"
#include "critical.h"
#include "Actividad3-Driver.h"
#include "Cronometro.h"

// Initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
Cronometro cronometro(lcd);

void onKeyDown(int tecla);
void onKeyUp(int tecla);

void setup() 
{
    lcd.begin(16,2);
    analogWrite(10, 200); // Controla intensidad backlight

    delay(1000);
    lcd.clear();
    
    lcd.print("ADC con ISR");
    
    byte customChar[8] = { B01110, B10101, B11111, B01110, B10001, B01010, B00100, B01010 };
          
    lcd.createChar(0, customChar);
    lcd.setCursor(0, 1); lcd.print(" ");
    lcd.write(byte(0)); lcd.print("  ");
    lcd.write(byte(0)); lcd.print("  ");
    lcd.write(byte(0)); lcd.print("  ");
    lcd.write(byte(0)); lcd.print("  ");
    lcd.write(byte(0));

    delay(5000);
    lcd.clear();

    cli(); // deshabilitar interrupciones globales
    fnqueue_init();
    ADC_init();
    TIMER_init();
    sei();  // habilitar interrupciones globales

    key_down_callback(onKeyDown);
    key_up_callback(onKeyUp);
}

void loop()
{
    // Ejecuta funciones encoladas
    fnqueue_run();
    cronometro.actualizar();
}

void onKeyDown(int tecla) {
    if (tecla == 1) { // UP -> alternar estado
        if (cronometro.estaCorriendo()) {
            cronometro.pausar();
        } else {
            cronometro.iniciar();
        }
    }
    if (tecla == 3) { // DOWN -> reset
        cronometro.reiniciar();
    }
}

void onKeyUp(int tecla) {
    // Nada por ahora
}

