#include <stdbool.h>
#include <stdint.h>
#include <Arduino.h>
#include <LiquidCrystal.h>
#include "fnqueue.h"
#include "critical.h"
#include "Actividad3-Driver.h"

// Initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void onKeyDown(int tecla);
void onKeyUp(int tecla);

void setup() 
{
    lcd.begin(16,2);
    analogWrite(10, 200); // Controla intensidad backlight

    lcd.clear();
    lcd.print("ADC con ISR "); 
    byte customChar[8] = { B01110, B10101, B11111, B01110, B10001, B01010, B00100, B01010 };
    lcd.createChar(0, customChar);
    lcd.setCursor(12, 0);
    lcd.write(byte(0)); 
    delay(2000);
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.autoscroll(); 
    const char *msg = "BUEN DIA! ESTA ES LA CATEDRA DE SISTEMAS EMBEBIDOS DE MANUEL TAURO Y GONZALO AGUIRRE";
    lcd.print(msg);
    delay(1000);
    lcd.noAutoscroll();

    cli(); // deshabilitar interrupciones globales
    fnqueue_init();
    ADC_init();
    TIMER_init();
    sei();  // habilitar interrupciones globales

    key_down_callback(onKeyDown);
    //key_up_callback(onKeyUp);
}

void loop()
{
    // Ejecuta funciones encoladas
    fnqueue_run();

    lcd.setCursor(0,0);
    lcd.print(contador);
}

void onKeyDown(int tecla) {
    

    switch (tecla)
    {
    case 4:
        if (!run)
            run = 1;
        else if (run)
            run = 0;
        break;
    case 3:
        if (run) {
            run = 0;
            delay(50);
            contador = 0;
        }
        break;
    default:
        break;
    }
}

// void onKeyUp(int tecla) {
//     // Nada por ahora
// }
