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
volatile long selectPressTime = 0; // tiempo de inicio de presion de la tecla SELECT

bool MCAmode = false; // modo de medición continua ascendente
bool MPmode = false; // modo de pausa
bool MVTmode = false; // modo de visor de tiempos
bool MADmode = false; // modo de ajuste de dimmer 
uint8_t upordown = 3; // 1 = UP, 2 = DOWN, 3 = FIRST CALL

// Variables globales del MAD
volatile long lastMadActivity = 0;
const uint8_t valores_dimmer[5] = {51, 102, 153, 204, 255}; //Los valores para el dimmer
volatile uint8_t dimmer_index = 3; // empieza en 80%

void onKeyDown(int tecla);
void onKeyUp(int tecla);
void displayLargeText(String text, int cursorCol, int cursorRow);
void storeTimeOnBuffer();
void showBufferedTime(uint8_t upordown);
void checkMadTimeOut();

void setup() 
{
    // Serial.begin(9600);
    // Serial.println("Iniciando...");

    lcd.begin(16,2);
    analogWrite(10, valores_dimmer[dimmer_index]); // Controla intensidad backlight

    lcd.clear();           // limpia toda la pantalla
    lcd.setCursor(0,0);
    String msg = "BUEN DIA!          ";
    displayLargeText(msg, 0, 0);
    lcd.clear();
    msg = "ESTA ES LA CATEDRA DE SISTEMAS EMBEBIDOS DE MANUEL TAURO Y GONZALO AGUIRRE, DEL 2DO CUATRIMESTRE DE 2025        ";
    displayLargeText(msg, 0, 0);
    lcd.clear();
    msg = "PRESIONE CUALQUIER TECLA PARA INICIAR EL CRONOMETRO           ";
    displayLargeText(msg, 0, 0);
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
    fnqueue_run();
    if (!MVTmode)
        mostrarCronometro(lcd);
    if(MADmode)
        checkMadTimeOut();
}

void onKeyDown(int tecla) {

    // Si el cronómetro estaba detenido, cualquiera de las 3 teclas (UP, DOWN, SELECT) lo inicia
    if (!timerIniciado && (tecla == 1 || tecla == 2 || tecla == 4)) {
        MCAmode = true;
        lcd.setCursor(0, 0); lcd.clear(); lcd.print("MCA MODE");
        run = 1;

        critical_begin();
        contador = 0;
        critical_end();
        
        timerIniciado = true;
        return;
    }

    switch (tecla)
    {

    case 1: // TECLA UP
        
        // Si está en MCA o MP, alterna entre ambos con tecla UP
        if (MCAmode) {
            run = !run; 
            MCAmode = false; 
            MPmode = true;
            lcd.setCursor(0, 0); lcd.clear(); lcd.print("MP MODE");
        } else if (MPmode) {
            run = !run; 
            MCAmode = true;
            MPmode = false;
            lcd.setCursor(0, 0); lcd.clear(); lcd.print("MCA MODE");
        } 

        // Si está en MVT, muestra el siguiente tiempo almacenado en buffer con tecla UP
        else if (MVTmode) {
            lcd.clear();
            lcd.setCursor(0, 0); lcd.print("MVT UP");
            upordown = 1;
            showBufferedTime(upordown);
        }

        else if(MADmode){
            lastMadActivity = millis(); // Actualizo el tiempo de presion
            lcd.clear(); lcd.setCursor(0,0); lcd.print("MAD MODE");

            if(dimmer_index < 4) 
                dimmer_index++;
            
            analogWrite(10,valores_dimmer[dimmer_index]);
            lcd.setCursor(0,0);lcd.print("MAD MODE  ");  
            lcd.print(valores_dimmer[dimmer_index] * 100 / 255); // 0,20,40,...100%
            lcd.print("%");
        }

        break;

    case 2: // TECLA DOWN

        // Si está en MCA o MP, guarda el tiempo actual en buffer con tecla DOWN
        if (MCAmode || MPmode) {
            if (run)
            {
                storeTimeOnBuffer();
            } else if (!run && timerIniciado)
            {
                storeTimeOnBuffer();
                run = 0; 

                critical_begin();
                contador = 0;
                critical_end();

                lcd.setCursor(0, 0); lcd.clear(); lcd.print("MP MODE");
            }
        } 
        
        // Si está en MVT, muestra el tiempo anterior almacenado en buffer con tecla DOWN
        else if (MVTmode) {
            lcd.clear();
            lcd.setCursor(0, 0); lcd.print("MVT DOWN");
            upordown = 2;
            showBufferedTime(upordown);
        }

        else if(MADmode){
            lastMadActivity = millis();
            lcd.setCursor(0,0);
            lcd.clear();

            if(dimmer_index > 0) 
                dimmer_index--;
            
            analogWrite(10,valores_dimmer[dimmer_index]);
            lcd.setCursor(0,0);lcd.print("MAD MODE  ");  
            lcd.print(valores_dimmer[dimmer_index] * 100 / 255); // 0,20,40,...100%
            lcd.print("%");

            }
        break;

    case 4: // TECLA SELECT
    
        // Si esta en MP, guarda el tiempo actual en selectPressTime con tecla SELECT
        if (MPmode) {
            selectPressTime = millis();
        }
        break;

    default:
        break;
    }

    // Debug information
    // Serial.println(" onkeydown ");
    // Serial.print(" MCA: "); Serial.println(MCAmode);
    // Serial.print(" MP: "); Serial.println(MPmode);
    // Serial.print(" MVT: "); Serial.println(MVTmode);
    // Serial.print(" MAD: "); Serial.println(MADmode);
    // Serial.println();

}

void onKeyUp(int tecla) {

    switch (tecla)
    {
    
    case 4: 
        // Al soltar SELECT, dependiendo del tiempo de presión, cambia de MP a MVT o MAD

        // En MP, si se presiona menos de 3 segundos, cambia a MVT. Si se presiona 3 segundos o más, cambia a MAD
        if (MPmode) {
            unsigned long pressDuration = millis() - selectPressTime;  // Calcular duración de la presión
            
            if (pressDuration < 3000) {  // Presión corta (<3s): cambiar a MVT
                MPmode = false;
                MVTmode = true;
                lcd.setCursor(0, 0); lcd.clear(); lcd.print("MVT MODE");

                // Como se ingresó a MVT desde MP, mostrar el tiempo almacenado 
                bufIndRow = 0; // resetear índice de fila
                showBufferedTime(upordown);
            } else {  // Presión larga (>=3s): cambiar a MAD (según consigna)
                MPmode = false;
                MADmode = true;
                lastMadActivity = millis();
                lcd.clear();
                lcd.setCursor(0,0);lcd.print("MAD MODE  ");  
                lcd.print(valores_dimmer[dimmer_index] * 100 / 255); // 0,20,40,...100%
                lcd.print("%");
            }
        } 
        
        // En MVT o MAD, al soltar SELECT, independientemente del tiempo transcurrido, vuelve a MP
        else if (MVTmode) {
            MVTmode = false;
            MPmode = true;
            lcd.setCursor(0, 0); lcd.clear(); lcd.print("MP MODE");
            upordown = 3; // resetear a FIRST CALL
        } else if (MADmode) {
            MADmode = false;
            MPmode = true;
            lcd.setCursor(0, 0); lcd.clear(); lcd.print("MP MODE");
            upordown = 3; // resetear a FIRST CALL
        }
        break;

    default:
        break;
    }
    
    // Debug information
    // Serial.println(" onkeyup ");
    // Serial.print(" MCA: "); Serial.println(MCAmode);
    // Serial.print(" MP: "); Serial.println(MPmode);
    // Serial.print(" MVT: "); Serial.println(MVTmode);
    // Serial.print(" MAD: "); Serial.println(MADmode);
    // Serial.println();

}


// Función para mostrar texto grande con efecto de escritura
// Si el texto es mayor a 16 caracteres, hace scroll automático
void displayLargeText(String text, int cursorCol, int cursorRow) {
    lcd.setCursor(cursorCol, cursorRow);
    int typewriterDelay = 130; // Retardo para efecto de escritura
    int j = 0;
    for (int i = 0; i < text.length(); i++) {
        if (i < 15) {
            lcd.setCursor(i, 0);
            lcd.print(text.charAt(i));
            delay(typewriterDelay); 
        } else {
            for (i = 16; i < text.length(); i++) {
                j++;
                lcd.setCursor(0, 0);
                lcd.print(text.substring(j, j+16));
                delay(typewriterDelay);
            }
        }
    }
}

// Función para verificar tiempo de inactividad en MAD y salir si supera 10 segundos
//esta es la funcion que llamo desde el loop para controlar q el tiempo no supere a los 5s sin actividad
void checkMadTimeOut(){
    if(MADmode){
        if(millis() - lastMadActivity >= 5000){
            MADmode = false;
            MPmode = true;
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("MP MODE"); 
        }
    }
}

// Función para almacenar el tiempo actual en el buffer
void storeTimeOnBuffer() {
    if (bufIndCol < 10) { 

        // Toma la variable CONTADOR proveniente del driver, la convierte a formato "m.ss.d" y la almacena en el buffer
        critical_begin();
        uint32_t currentTime = contador;
        critical_end();
        
        uint32_t minutes = currentTime / 600;
        uint32_t seconds = (currentTime / 10) % 60;
        uint32_t deciseconds = currentTime % 10;
        sprintf(storingBuffer[bufIndCol], "%01lu.%02lu.%1lu", minutes, seconds, deciseconds);
        bufIndCol++;

        // Muestra en LCD el tiempo almacenado recientemente por 1.5 segundos
        String printingmessage = "Mem" + String(bufIndCol) + ": ";
        lcd.setCursor(0, 1); lcd.print(printingmessage); lcd.setCursor(printingmessage.length(),1); lcd.print(storingBuffer[bufIndCol-1]);
        delay(600);
        lcd.setCursor(0, 1); lcd.print("                "); // limpiar línea
    } else {
        // Buffer lleno, mostrar mensaje de error
        lcd.setCursor(0, 1); lcd.print("Buffer Full "); delay(200); lcd.setCursor(0, 1); lcd.print("            "); delay(200);
        lcd.setCursor(0, 1); lcd.print("Buffer Full "); delay(200); lcd.setCursor(0, 1); lcd.print("            "); delay(200);
        lcd.setCursor(0, 1); lcd.print("Buffer Full "); delay(200); lcd.setCursor(0, 1); lcd.print("            "); delay(200);
    }
}

// Función para mostrar tiempos almacenados en buffer en modo MVT
// upordown = 1 -> UP, upordown = 2 -> DOWN, upordown = 3 -> FIRST CALL
void showBufferedTime(uint8_t upordown) {
    
    // Debug information
    // Serial.println(" showBufferedTime ");
    // Serial.print(" bufIndCol: "); Serial.println(bufIndCol);
    // Serial.print(" bufIndRow: "); Serial.println(bufIndRow);
    // Serial.println();
    
    // Si no hay tiempos almacenados, mostrar mensaje de error y salir
    if (bufIndCol == 0) {
        lcd.setCursor(0, 1); lcd.print("No Mem "); delay(200); lcd.setCursor(0, 1); lcd.print("       "); delay(200);
        lcd.setCursor(0, 1); lcd.print("No Mem "); delay(200); lcd.setCursor(0, 1); lcd.print("       "); delay(200);
        lcd.setCursor(0, 1); lcd.print("No Mem "); delay(200); lcd.setCursor(0, 1); lcd.print("       "); delay(200);
        return;
    }

    // En la primera llamada, mostrar el primer tiempo almacenado
    if (upordown == 3) {
        lcd.setCursor(0, 1); lcd.print("                ");
        lcd.setCursor(0, 1); lcd.print("Mem" + String(bufIndRow + 1) + ": " + String(storingBuffer[bufIndRow]) + "   ");
        return;
    }
    
    // Actualizar índice de fila según dirección (UP/DOWN) y límites del buffer
    if (upordown == 1) { // UP
        if (bufIndRow < bufIndCol - 1) {
            bufIndRow++;
        } // si ya está en el último, no hacer nada
        
    } else if (upordown == 2){ // DOWN
        if (bufIndRow > 0) {
            bufIndRow--;
        } // si ya está en el primero, no hacer nada
        
    }

    // Mostrar siempre la memoria actual
    lcd.setCursor(0, 1); lcd.print("                ");
    lcd.setCursor(0, 1); lcd.print("Mem" + String(bufIndRow + 1) + ": " + String(storingBuffer[bufIndRow]) + "   ");
}
