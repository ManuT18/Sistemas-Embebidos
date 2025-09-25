#ifndef ADC_H
#define ADC_H

#include <stdint.h>

// Variables globales externas (si se necesitan acceder desde otros archivos)
extern volatile uint16_t adc_value;
extern uint8_t NUM_KEYS;
extern uint16_t adc_key_val[5];

// Declaraciones de funciones públicas para el manejo del ADC y teclado
void ADC_init(void);
void key_down_callback(void (*handler)(int tecla));
void key_up_callback(void (*handler)(int tecla));

#endif // ADC_H
