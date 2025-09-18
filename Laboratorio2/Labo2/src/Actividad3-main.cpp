#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <Arduino.h>
#include <LiquidCrystal.h>
#include "fnqueue.h"
#include "critical.h"
#include "Actividad3-Driver.h"

// Initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Variables globales internas 
// char msgs[5][17] = { " Right", " Up", " Down", " Left", " Select" };
volatile bool timerIniciado = false; // asegurarnos de iniciar el timer solo una vez
char storingBuffer[10][9]; // buffer para almacenar hasta 10 tiempos en formato "m.ss.d"
int bufIndCol = 0, bufIndRow = 0;
unsigned long startTime = 0;
volatile long selectPressTime = 0; // tiempo de inicio de presion de la tecla SELECT

bool MCAmode = false; // modo de medición continua ascendente
bool MPmode = false; // modo de pausa
bool MVTmode = false; // modo de visor de tiempos
bool MADmode = false; // modo de ajuste de dimmer 

void onKeyDown(int tecla);
void onKeyUp(int tecla);
void displayLargeText(String text, int cursorCol, int cursorRow);
void storeTimeOnBuffer();

void setup() 
{
    startTime = millis();

    Serial.begin(9600);
    Serial.println("Iniciando...");

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
    key_up_callback(onKeyUp);
}

void loop()
{
    fnqueue_run();
    mostrarCronometro(lcd);
}

void onKeyDown(int tecla) {
    // Si el cronómetro estaba detenido, cualquier tecla lo inicia
    if (!timerIniciado && !run) {
        MCAmode = true;
        lcd.setCursor(0, 0); lcd.clear(); lcd.print("MCA");
        run = 1;
        contador = 0;
        timerIniciado = true;
        return;
    }

    switch (tecla)
    {
    case 1: // Alterna entre iniciar y pausar el cronómetro con tecla UP
        if (MCAmode) {
            run = !run; 
            MCAmode = false; 
            MPmode = true;
            if (!run) {
                lcd.setCursor(0, 0); lcd.clear(); lcd.print("MP");
            }
        } else if (MPmode) {
            MCAmode = true;
            MPmode = false;
            run = !run; 
            if (run) {
                lcd.setCursor(0, 0); lcd.clear(); lcd.print("MCA");
            }
        } else if (MVTmode) {
            lcd.clear();
            lcd.setCursor(0, 0); lcd.print("MVT UP");
        }
        break;

    case 2: // Almacena el tiempo actual en buffer con tecla DOWN
        if (MCAmode || MPmode) {
            if (run)
            {
                storeTimeOnBuffer();
            } else if (!run && timerIniciado)
            {
                storeTimeOnBuffer();
                run = 0; 
                contador = 0;
                lcd.setCursor(0, 0); lcd.clear(); lcd.print("MP");
            }
        } else if (MVTmode) {
            lcd.clear();
            lcd.setCursor(0, 0); lcd.print("MVT DOWN");
        }
        break;

    case 4: // Cambios de modo con tecla SELECT
        if (MPmode) {
            selectPressTime = millis();
        }
        break;

    default:
        break;
    }

    Serial.println(" onkeydown ");
    Serial.print(" MCA: "); Serial.println(MCAmode);
    Serial.print(" MP: "); Serial.println(MPmode);
    Serial.print(" MVT: "); Serial.println(MVTmode);
    Serial.print(" MAD: "); Serial.println(MADmode);
    Serial.println();
}

void onKeyUp(int tecla) {
    switch (tecla)
    {
    case 4: // Cambios de modo con tecla SELECT (al soltar)
        if (MPmode) {
            unsigned long pressDuration = millis() - selectPressTime;  // Calcular duración de la presión
            if (pressDuration < 3000) {  // Presión corta (<3s): cambiar a MVT
                MPmode = false;
                MVTmode = true;
                lcd.setCursor(0, 0); lcd.clear(); lcd.print("MVT");
            } else {  // Presión larga (>=3s): cambiar a MAD (según consigna)
                MPmode = false;
                MADmode = true;
                lcd.setCursor(0, 0); lcd.clear(); lcd.print("MAD");
            }
        } else if (MVTmode) {
            MVTmode = false;
            MPmode = true;
            lcd.setCursor(0, 0); lcd.clear(); lcd.print("MP");
        } else if (MADmode) {
            MADmode = false;
            MPmode = true;
            lcd.setCursor(0, 0); lcd.clear(); lcd.print("MP");
        }
        break;

    default:
        break;
    }
    
    Serial.println(" onkeyup ");
    Serial.print(" MCA: "); Serial.println(MCAmode);
    Serial.print(" MP: "); Serial.println(MPmode);
    Serial.print(" MVT: "); Serial.println(MVTmode);
    Serial.print(" MAD: "); Serial.println(MADmode);
    Serial.println();
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

void storeTimeOnBuffer() {
    if (bufIndCol < 10) { 
        uint32_t currentTime = contador;
        uint32_t minutes = currentTime / 600;
        uint32_t seconds = (currentTime / 10) % 60;
        uint32_t deciseconds = currentTime % 10;
        sprintf(storingBuffer[bufIndCol], "%01lu.%02lu.%1lu", minutes, seconds, deciseconds);
        bufIndCol++;

        String printingmessage = "Mem" + String(bufIndCol) + ": ";
        lcd.setCursor(0, 1); lcd.print(printingmessage); lcd.setCursor(printingmessage.length(),1); lcd.print(storingBuffer[bufIndCol-1]);
        delay(1500);
        lcd.setCursor(0, 1); lcd.print("                "); // limpiar línea
    } else {
        lcd.setCursor(0, 1); lcd.print("Buffer Full "); delay(200); lcd.setCursor(0, 1); lcd.print("            "); delay(200);
        lcd.setCursor(0, 1); lcd.print("Buffer Full "); delay(200); lcd.setCursor(0, 1); lcd.print("            "); delay(200);
        lcd.setCursor(0, 1); lcd.print("Buffer Full "); delay(200); lcd.setCursor(0, 1); lcd.print("            "); delay(200);
    }
}