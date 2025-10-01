#include "ldr_driver.h"
#include "adc_driver.h"
#include "fnqueue.h"
#include "teclado_driver.h"
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <stdio.h>

// --------------------------------------------------
// Callbacks
// --------------------------------------------------
uint16_t last_print = 0;
uint16_t interval = 1000; 

// Asignación de callbacks específicos por tecla para key down y key up
void on_key0_down() { on_keyboard_down_event(0); }
void on_key1_down() { on_keyboard_down_event(1); }
void on_key2_down() { on_keyboard_down_event(2); }
void on_key3_down() { on_keyboard_down_event(3); }
void on_key4_down() { on_keyboard_down_event(4); }

void on_key0_up() { on_keyboard_up_event(0); }
void on_key1_up() { on_keyboard_up_event(1); }
void on_key2_up() { on_keyboard_up_event(2); }
void on_key3_up() { on_keyboard_up_event(3); }
void on_key4_up() { on_keyboard_up_event(4); }

// Callback del LDR (recibe valor de lux)
void on_ldr_update(float lux) {
    uint16_t now = millis();
    if (now - last_print >= interval) {
        last_print = now;
        Serial.print("Luz medida: ");
        Serial.print(lux);
        Serial.println(" LUX");
    }
}

// Callback genérico que imprime la tecla (recibe índice)
void on_keyboard_down_event(int tecla) {
    Serial.print("Tecla ");
    Serial.print(tecla);
    Serial.println(" presionada");
}

// Callback genérico que imprime la tecla (recibe índice)
void on_keyboard_up_event(int tecla) {
    Serial.print("Tecla ");
    Serial.print(tecla);
    Serial.println(" soltada");
}

// --------------------------------------------------
// Configuraciones globales de los drivers
// --------------------------------------------------
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // Declare LCD with pins (adjust as needed)
ldr_cfg_t ldr_conf;
float lux_actual = 0;
unsigned long last_upadate = 0;
//const unsigned long interval = 500;

void setup() {
    

    // -----------------------------
    // Inicialización del Serial y LCD
    // -----------------------------
    Serial.begin(9600);
    delay(1000); 
    Serial.println("Sistema iniciado");
    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Iniciado");

    // -----------------------------
    // Inicialización del LDR
    // -----------------------------
    ldr_conf.channel = 1; 
    ldr_conf.callback = on_ldr_update;
    ldr_init(&ldr_conf);

    // -----------------------------
    // Inicialización del teclado
    // -----------------------------
    keyboard_init();

    // Registrar handlers por tecla (0..4) para key down y key up
    key_down_callback(on_key0_down, 0);
    key_down_callback(on_key1_down, 1);
    key_down_callback(on_key2_down, 2);
    key_down_callback(on_key3_down, 3);
    key_down_callback(on_key4_down, 4);

    key_up_callback(on_key0_up, 0);
    key_up_callback(on_key1_up, 1);
    key_up_callback(on_key2_up, 2);
    key_up_callback(on_key3_up, 3);
    key_up_callback(on_key4_up, 4);
}

void loop() {

    fnqueue_run();

}
