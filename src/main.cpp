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
unsigned long last_print = 0;
const unsigned long interval = 1000; 

void on_ldr_update(float lux) {
    unsigned long now = millis();
    if (now - last_print >= interval) {
        last_print = now;
        Serial.print("Luz medida: ");
        Serial.println(lux);
    }
}

// Callback genérico que imprime la tecla (recibe índice)
void on_keyboard_down_event(int tecla) {
    Serial.print("Tecla ");
    Serial.print(tecla);
    Serial.println(" presionada");
}

void on_keyboard_up_event(int tecla) {
    Serial.print("Tecla ");
    Serial.print(tecla);
    Serial.println(" soltada");
}

// Wrappers sin parámetros (API del teclado requiere funciones void())
void on_key0_down() { on_keyboard_down_event(0); }
void on_key1_down() { on_keyboard_down_event(1); }
void on_key2_down() { on_keyboard_down_event(2); }
void on_key3_down() { on_keyboard_down_event(3); }
void on_key4_down() { on_keyboard_down_event(4); }

// (Opcional) wrappers para key up
void on_key0_up() { on_keyboard_up_event(0); }
void on_key1_up() { on_keyboard_up_event(1); }
void on_key2_up() { on_keyboard_up_event(2); }
void on_key3_up() { on_keyboard_up_event(3); }
void on_key4_up() { on_keyboard_up_event(4); }

// --------------------------------------------------
// Configuraciones globales de los drivers
// --------------------------------------------------
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // Declare LCD with pins (adjust as needed)
ldr_cfg_t ldr_conf;
float lux_actual = 0;
unsigned long last_upadate = 0;
//const unsigned long interval = 500;

void setup() {
    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Iniciando...");

    Serial.begin(9600);
    delay(1000); 
    Serial.println("Sistema iniciado");
    
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
    // Registrar handlers por tecla (0..4)
    key_down_callback(on_key0_down, 0);
    key_down_callback(on_key1_down, 1);
    key_down_callback(on_key2_down, 2);
    key_down_callback(on_key3_down, 3);
    key_down_callback(on_key4_down, 4);

    // (Opcional) key up si querés detectar la suelta
    key_up_callback(on_key0_up, 0);
    key_up_callback(on_key1_up, 1);
    key_up_callback(on_key2_up, 2);
    key_up_callback(on_key3_up, 3);
    key_up_callback(on_key4_up, 4);
}

void loop() {

    fnqueue_run();

}
