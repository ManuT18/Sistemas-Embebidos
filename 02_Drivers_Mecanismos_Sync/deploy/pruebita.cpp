#include <avr/io.h>
#include <avr/interrupt.h>
#include <LiquidCrystal.h>
#include "fnqueue.h"
#include "critical.h"
#include "Arduino.h"

// LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Variables globales
volatile uint16_t adc_value = 0;

void ADC_init(void) {
    // Referencia AVCC, canal ADC0
    // El ADC medirá el voltaje presente en el pin A0, usando AVCC (5v) como referencia
    // ADMUX configura referencia y canal del ADC
    ADMUX = (1 << REFS0);

    // En ADCSRA configuramos el prescaler para que trabaje de forma segura entre 50 y 200 kHz
    // El reloj del Arduino UNO es de 16MHz, demasiado rápido para el ADC, así que hay que dividirlo
    //
    // Prescaler = 128 (16MHz/128 = 125kHz)
    // 2,      4,        8,        16,       32,       64,       128
    // 001,    010,      011,      100,      101,      110,      111
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    // Habilitar el módulo ADC (ADEN) y habilita la interrupción que se dispara al terminar cada conversión (genera ADC_vect) (ADIE)
    ADCSRA |= (1 << ADEN) | (1 << ADIE);

    // Free running
    // El ADC lanzará conversiones continuamente sin que haya que poner ADSC cada vez
    ADCSRA |= (1 << ADATE);

    // Arrancar conversión
    // ADSC inicia la primera conversión
    // En Free Running, al terminar una conversión ADSC se vuelve a iniciar automáticamente.
    ADCSRA |= (1 << ADSC);
}

// ISR del ADC
ISR(ADC_vect) {
    // Leer valor ADC
    // Lectura ordenada: primero ADCL y luego ADCH
    // “bloquear” correctamente el par de bytes y no perder la coherencia del resultado
    uint8_t low  = ADCL;
    uint8_t high = ADCH;

    // Combinar ambos bytes high y low en un solo valor de 10 bits
    // El resultado del ADC es un valor entre 0 y 1023 (10 bits)
    adc_value = (high << 8) | low;

    // Aquí podrías encolar tarea con fnqueue_add(...)
}

void setup() {
    lcd.begin(16, 2);
    lcd.print("ADC con ISR");

    fnqueue_init();
    ADC_init();
    sei();  // habilitar interrupciones globales
}

void loop() {
    // Ejecuta funciones encoladas
    fnqueue_run();

    // Ejemplo: mostrar valor en LCD
    lcd.setCursor(0, 1);
    lcd.print("Valor ADC: ");
    lcd.setCursor(10, 1);
    lcd.print(adc_value);
    lcd.print("   "); // Limpiar restos
}

