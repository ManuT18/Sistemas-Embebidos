#include <Arduino.h>
#include <LiquidCrystal.h>
#include "fnqueue.h"
#include "Actividad2-Driver.h"
#include "Cronometro.h"

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
Cronometro cronometro(lcd);

void onKeyDown(int tecla);
void onKeyUp(int tecla);

ISR(TIMER1_COMPA_vect) {
    cronometro.incrementar();
}

void setup() {
    lcd.begin(16,2);
    analogWrite(10, 255);

    cli();
    TCCR1A = 0;
    TCCR1B = 0;
    OCR1A = 15624/10;
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS12) | (1 << CS10);
    TIMSK1 |= (1 << OCIE1A);
    sei();

    fnqueue_init();
    ADC_init();
    key_down_callback(onKeyDown);
    key_up_callback(onKeyUp);
}

void loop() {
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
    // nada por ahora
}
