#include "ldr.h"
#include "adc_driver.h"
#include "fnqueue.h"
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

// --------------------------------------------------
// Configuraciones globales de los drivers
// --------------------------------------------------
ldr_cfg_t ldr_conf;
float lux_actual = 0;
unsigned long last_upadate = 0;
const unsigned long interval = 500;

void setup() {
    Serial.begin(9600);
    delay(1000); 
    Serial.println("Sistema iniciado");
    // -----------------------------
    // Inicialización del LDR
    // -----------------------------
    ldr_conf.channel = 0; 
    ldr_conf.callback = on_ldr_update;
    ldr_init(&ldr_conf);
}

void loop() {

    fnqueue_run();
}
