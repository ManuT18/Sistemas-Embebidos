#include <stdbool.h>
#include <stdint.h>
#include <Arduino.h>
#include <LiquidCrystal.h>
#include "fnqueue.h"
#include "critical.h"
#include "Cronometro.h"

// Variables globales internas del driver
uint8_t NUM_KEYS = 5;
uint16_t adc_key_val[5] = {30, 150, 360, 535, 760};

static int last_key = -1;
volatile uint16_t adc_value = 0;

#define MAX_HANDLERS 5 
static void (*key_down_handlers[MAX_HANDLERS])(int tecla);
static void (*key_up_handlers[MAX_HANDLERS])(int tecla);
static int num_key_down = 0;
static int num_key_up = 0; 

void key_event_handler();
int get_key(uint16_t adc_value);
void ADC_init(void);
ISR(ADC_vect);
ISR(TIMER1_COMPA_vect);

// Registrar handlers
void key_down_callback(void (*handler)(int tecla))
{
	key_down_handlers[num_key_down++] = handler;
}

void key_up_callback(void (*handler)(int tecla))
{
	key_up_handlers[num_key_up++] = handler;
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

void TIMER_init(void)
{
	// Configurar Timer1 para que genere una interrupción cada 100ms

	// Limpia cualquier configuración previa de los registros Timer Counter Control Register 1 A y B
	// El registro A controla aspectos como el modo de onda y las salidas de comparación
	// El registro B controla el prescaler, modo de generación de ondas y fuente de reloj
    	TCCR1A = 0;
    	TCCR1B = 0;

	// Asigna el valor de comparación de 1562 = 15624/10 al registro Out Compare Register 1 A
	// El valor 15624 se obtiene de la fórmula:
	// { 16MHz / 1024 (prescaler) / 1Hz (frecuencia deseada) } - 1 = 1562
    	OCR1A = 15624/10;

	// Setea el bit WGM12 (Waveform Generation Mode bit 12) en TCCR1B para configurar el Timer1 en modo CTC (Clear Timer on Compare Match)
	// En este modo, el Timer1 se reinicia a 0 cada vez que alcanza el valor en OCR1A (1562)
	// Esto permite generar interrupciones periódicas con precisión y que el timer no cuente indefinidamente
    	TCCR1B |= (1 << WGM12);

	// Setea los bits CS12 y CS10 en TCCR1B para seleccionar un prescaler de 1024
	// El prescaler divide la frecuencia del reloj del sistema (16MHz) para que el Timer1 cuente a una velocidad más manejable
	// Con un prescaler de 1024, el Timer1 contará a 16MHz/1024 = 15625Hz
	// Esto significa que el Timer1 incrementará su valor en 1 cada 64 microsegundos
    	TCCR1B |= (1 << CS12) | (1 << CS10);

	// Setea el bit OCIE1A (Output Compare Match Interrupt Enable) en el registro Timer Interrupt Mask Register 1 (TIMSK1)
	// Esto habilita la interrupción que se genera cuando el Timer1 alcanza el valor en OCR1A
	// Cuando esto ocurre, se ejecuta la rutina de servicio de interrupción asociada (ISR(TIMER1_COMPA_vect))
	// Esta interrupción se utilizará para incrementar el cronómetro cada 100ms
    	TIMSK1 |= (1 << OCIE1A);
}

// Función que traduce valor ADC en tecla
int get_key(uint16_t adc_value)
{
	for (int i = 0; i < NUM_KEYS; i++)
	{
		if (adc_value <= adc_key_val[i])
		{
			return i; 
		}
	}
	return -1; // No key pressed
}

ISR(ADC_vect)
{
	adc_value = ADC;             // leer valor crudo

	fnqueue_add(key_event_handler);  // encolar el handler
}

ISR(TIMER1_COMPA_vect) {
	cronometro.incrementar();
} 

void key_event_handler()
{
	int tecla = get_key(adc_value);

	delay(50); // debounce

	if (tecla != last_key)
	{
		if (tecla != -1)
          {
			for (int i = 0; i < num_key_down; i++)
			{
				key_down_handlers[i](tecla); // ejecuta todos los handlers registrados
			}
		}
		if (last_key != -1)
		{
			for (int i = 0; i < num_key_up; i++)
			{
				key_up_handlers[i](last_key);
			}
		}
		last_key = tecla;
	}
}