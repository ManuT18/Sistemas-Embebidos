#include <stdbool.h>
#include <stdint.h>
#include <Arduino.h>
#include <LiquidCrystal.h>
#include "fnqueue.h"
#include "critical.h"
#include "Actividad3-Driver.h"

// Initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
volatile bool timerIniciado = false; // asegurarnos de iniciar el timer solo una vez

void onKeyDown(int tecla);
void onKeyUp(int tecla);
void displayLargeText(String text, int cursorCol, int cursorRow);

void setup() 
{
    lcd.begin(16,2);
    analogWrite(10, 200); // Controla intensidad backlight

    lcd.clear();           // limpia toda la pantalla
    lcd.setCursor(0,0);
    String msg = "BUEN DIA!          ";
    displayLargeText(msg, 0, 0);
    lcd.clear();
    // msg = "ESTA ES LA CATEDRA DE SISTEMAS EMBEBIDOS DE MANUEL TAURO Y GONZALO AGUIRRE, DEL 2DO CUATRIMESTRE DE 2025        ";
    // displayLargeText(msg, 0, 0);
    // lcd.clear();
    // msg = "PRESIONE CUALQUIER TECLA PARA INICIAR EL CRONOMETRO           ";
    // displayLargeText(msg, 0, 0);
    // lcd.clear();

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
    fnqueue_run();
    mostrarCronometro(lcd);
}

void onKeyDown(int tecla) {
    // // Si el cronómetro estaba detenido, cualquier tecla lo inicia
    // if (!timerIniciado) {
    //     run = 1;
    //     contador = 0;
    //     TIMER_init();
    //     timerIniciado = true;
    // }
    switch (tecla)
    {
    case 1: // Alterna entre iniciar y pausar el cronómetro con tecla SELECT
        run = !run; 
        if (run) {
            lcd.setCursor(0, 0); lcd.clear(); lcd.print("MCA");
        } else {
            lcd.setCursor(0, 0); lcd.clear(); lcd.print("MP");
        }
        break;
    case 3: // Detiene el cronómetro con tecla LEFT y reinicia el contador
        run = 0; 
        contador = 0;
        break;
    default:
        
        break;
    }
}

void displayLargeText(String text, int cursorCol, int cursorRow) {
    lcd.setCursor(cursorCol, cursorRow);
    int j = 0;
    for (int i = 0; i < text.length(); i++) {
        if (i < 15) {
            lcd.setCursor(i, 0);
            lcd.print(text.charAt(i));
            delay(100); // Retardo para efecto de escritura
        } else {
            for (i = 16; i < text.length(); i++) {
                j++;
                lcd.setCursor(0, 0);
                lcd.print(text.substring(j, j+16));
                delay(100); // Retardo para efecto de escritura
            }
        }
    }
}