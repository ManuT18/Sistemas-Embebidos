#include "ldr_driver.h"
#include "adc_driver.h"
#include "fnqueue.h"
#include "teclado_driver.h"
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <stdio.h>


// -------------------
// Funciones auxiliares
// -------------------

// Declaración de funciones y callbacks
void on_ldr_update(float lux);
void on_keyboard_down_event(int tecla);
void update_lcd_display();
void process_serial_commands();
// void on_keyboard_up_event(int tecla);

// Asignación de callbacks específicos por tecla para key down y key up
void on_key0_down() { on_keyboard_down_event(0); }
void on_key1_down() { on_keyboard_down_event(1); }
void on_key2_down() { on_keyboard_down_event(2); }
void on_key3_down() { on_keyboard_down_event(3); }
void on_key4_down() { on_keyboard_down_event(4); }

// void on_key0_up() { on_keyboard_up_event(0); }
// void on_key1_up() { on_keyboard_up_event(1); }
// void on_key2_up() { on_keyboard_up_event(2); }
// void on_key3_up() { on_keyboard_up_event(3); }
// void on_key4_up() { on_keyboard_up_event(4); }


// -------------------
// Variables globales
// -------------------
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // Declare LCD with pins (adjust as needed)
ldr_cfg_t ldr_conf;
float lux_actual = 0;
unsigned long last_upadate = 0;
uint16_t last_print = 0;
uint16_t interval = 100;  // Intervalo de impresión en ms
enum { MODO_NORMAL = 0, MODO_AVG = 1, MODO_MAX = 2, MODO_MIN = 3 };
int modo_display = MODO_NORMAL;


void setup() {

    // -------------------
    // Inicialización del Serial y LCD
    // -------------------
    Serial.begin(9600);
    delay(1000); 
    Serial.println("Sistema iniciado");
    lcd.begin(16, 2);
    lcd.clear();

    cli(); // deshabilitar interrupciones globales

    // -------------------
    // Inicialización del LDR
    // -------------------
    ldr_conf.channel = 1; 
    ldr_conf.callback = on_ldr_update;
    ldr_init(&ldr_conf);

    // -------------------
    // Inicialización del teclado
    // -------------------
    keyboard_init();

    // -------------------
    // Inicialización de la cola de funciones
    // -------------------
    fnqueue_init();

    // Encolar la función de procesamiento de comandos seriales
    fnqueue_add(process_serial_commands);

    sei();  // habilitar interrupciones globales
    
    // Registrar handlers por tecla (0..4) para key down y key up
    key_down_callback(on_key0_down, 0);
    key_down_callback(on_key1_down, 1);
    key_down_callback(on_key2_down, 2);
    key_down_callback(on_key3_down, 3);
    key_down_callback(on_key4_down, 4);

    // key_up_callback(on_key0_up, 0);
    // key_up_callback(on_key1_up, 1);
    // key_up_callback(on_key2_up, 2);
    // key_up_callback(on_key3_up, 3);
    // key_up_callback(on_key4_up, 4);
}


void loop() {

    fnqueue_run();
    
}


// Calculo del promedio de la variable pasada por parametro, en base al minimo y maximo
int calculate_percentage(float value) {
    float min_lux = get_min_lux();
    float max_lux = get_max_lux();

    if (max_lux > min_lux) {
        return ((value - min_lux) / (max_lux - min_lux)) * 100.0f;
    } 
}

// Callback del LDR (recibe valor de lux)
void on_ldr_update(float lux) {
    uint16_t now = millis();
    if (now - last_print >= interval) {
        last_print = now;

        lux_actual = lux;  // actualizar variable global

        if (modo_display == MODO_NORMAL) {
            lcd.setCursor(0, 0);
            lcd.print("Midiendo luz");
            lcd.setCursor(0, 1);
            lcd.print(lux_actual); lcd.print(" LUX");
        }
        
        update_lcd_display();
    }
}


// Función para actualizar el display LCD según el modo actual
void update_lcd_display() {
    if (modo_display == MODO_AVG) {
        lcd.setCursor(0, 0);
        lcd.print("Avg Lux:");
        lcd.setCursor(0, 1);
        lcd.print(get_lux_avg()); lcd.print(" LUX = ");
        lcd.print(calculate_percentage(get_lux_avg())); lcd.print("%      ");
    } else if (modo_display == MODO_MAX) {
        lcd.setCursor(0, 0);
        lcd.print("Max Lux:");
        lcd.setCursor(0, 1);
        lcd.print(get_max_lux()); lcd.print(" LUX");
    } else if (modo_display == MODO_MIN) {
        lcd.setCursor(0, 0);
        lcd.print("Min Lux:");
        lcd.setCursor(0, 1);
        lcd.print(get_min_lux()); lcd.print(" LUX");
    }
}


// Función para procesar comandos del host por Serial
void process_serial_commands() {
    if (Serial.available()) {
        char c = Serial.read();

        // Cambiar modo según el comando
        if (c == '1') {
            modo_display = MODO_NORMAL;
        } else if (c == '2') {
            modo_display = MODO_MAX;
        } else if (c == '3') {
            modo_display = MODO_MIN;
        } else if (c == '4') {
            modo_display = MODO_AVG;
        }

        // Actualizar LCD inmediatamente
        lcd.clear();
        update_lcd_display();

        
    }

    // Re-encolar la función para que se ejecute nuevamente
    fnqueue_add(process_serial_commands);
}


// Callback genérico que imprime la tecla (recibe índice)
void on_keyboard_down_event(int tecla) {
    switch (tecla)
    {
    case 0: // TECLA RIGHT
        modo_display = MODO_AVG;
        lcd.clear();
        update_lcd_display();
        break;
    
    case 1: // TECLA UP
        modo_display = MODO_MAX;
        lcd.clear();
        update_lcd_display();
        break;

    case 2: // TECLA DOWN
        modo_display = MODO_MIN;
        lcd.clear();
        update_lcd_display();
        break;

    case 3: // TECLA LEFT
        
        break;

    case 4: // TECLA SELECT
        modo_display = MODO_NORMAL;
        lcd.clear();
        update_lcd_display();
        break;
        
    default:
        break;
    }
}


// // Callback genérico que imprime la tecla (recibe índice)
// void on_keyboard_up_event(int tecla) {
//     // No se usa en este ejemplo
// }
