#include <stdbool.h>
#include <stdint.h>
#include "Arduino.h"
#include <LiquidCrystal.h>
#include "fnqueue.h"
#include "critical.h"

// Variables globales internas del driver
uint8_t NUM_KEYS = 5;
uint16_t adc_key_val[5] = {30, 150, 360, 535, 760};

static void (*key_down_handler)(int tecla) = 0;
static void (*key_up_handler)(int tecla) = 0;
static int last_key = -1;
volatile uint16_t adc_value = 0;

void key_event_handler();
int get_key(uint16_t adc_value);
void ADC_init(void);
ISR(ADC_vect);
// void handler();

// Registrar handlers
void key_down_callback(void (*handler)(int tecla))
{
	key_down_handler = handler;
}

void key_up_callback(void (*handler)(int tecla))
{
	key_up_handler = handler;
}

void ADC_init(void)
{
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

// Función que traduce valor ADC en tecla
int get_key(uint16_t adc_value)
{
	if (adc_value < 50)  return 0; // RIGHT
	if (adc_value < 200) return 1; // UP
	if (adc_value < 400) return 2; // DOWN
	if (adc_value < 600) return 3; // LEFT
	if (adc_value < 800) return 4; // SELECT
	return -1;	// ninguna
}

ISR(ADC_vect)
{
	adc_value = ADC;             // leer valor crudo
	fnqueue_add(key_event_handler);  // encolar el handler
}

// Funciones para registrar callbacks
// void key_down_callback(void (*handler)(int tecla))
// {
// 	key_down_handler = handler;
// }

// void key_up_callback(void (*handler)(int tecla))
// {
// 	key_up_handler = handler;
// }

void key_event_handler()
{
	uint16_t value = adc_value;
	int tecla = get_key(adc_value);

	if (tecla != last_key) {
		if (tecla != -1 && key_down_handler) {
			key_down_handler(tecla);
		}
		if (last_key != -1 && key_up_handler) {
		    	key_up_handler(last_key);
		}
		last_key = tecla;
	}
	delay(1000); // anti rebote
}