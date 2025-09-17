#include <stdbool.h>
#include <stdint.h>
#include <Arduino.h>
#include <LiquidCrystal.h>
#include "fnqueue.h"
#include "critical.h"

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
static int db = 0; // contador antirrebote
static int tecla = -1; // tecla actual



//########################################################################################
//###																  ###
//###					DEFINICION DE FUNCIONES							  ###
//###																  ###
//########################################################################################




void key_event_handler();
int get_key(uint16_t adc_value);
void ADC_init(void);
ISR(ADC_vect);
ISR(TIMER0_COMPA_vect);
void incrementar();
void TIMER_init(void);


// Registrar handlers
void key_down_callback(void (*handler)(int tecla))
{
	if (num_key_down < MAX_HANDLERS)
		key_down_handlers[num_key_down++] = handler;
}

void key_up_callback(void (*handler)(int tecla))
{
	if (num_key_up < MAX_HANDLERS)
		key_up_handlers[num_key_up++] = handler;
}



//########################################################################################
//###																  ###
//###					CONFIGURACIONES DEL ADC							  ###
//###																  ###
//########################################################################################



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
	for (int i = 0; i < NUM_KEYS; i++)
	{
		if (adc_value < adc_key_val[i])
		{
			return i; 
		}
	}
	return -1; // No key pressed
}

ISR(ADC_vect)
{
	adc_value = ADC;  // leer valor crudo

	fnqueue_add(key_event_handler);  // encolar el handler
}

void key_event_handler()
{
	tecla = get_key(adc_value);
	
	db++;
	if (db >= 200) {
		db = 0;
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
}



//########################################################################################
//###																  ###
//###					CONFIGURACIONES DEL TIMER						  ###
//###																  ###
//########################################################################################



volatile uint32_t contador = 0;  
volatile uint8_t run = 0;  


void TIMER_init(void)
{
	// Configurar Timer0 para que genere una interrupción cada 100ms

	// Limpia cualquier configuración previa de los registros Timer Counter Control Register 0 A y B
	// El registro A controla aspectos como el modo de onda y las salidas de comparación
	// El registro B controla el prescaler, modo de generación de ondas y fuente de reloj
    	TCCR0A = 0;
    	TCCR0B = 0;

	// Asigna el valor de comparación de 156 = 15624/100 al registro Out Compare Register 0 A
	// El valor 15624 se obtiene de la fórmula:
	// { 16MHz / 1024 (prescaler) / 1Hz (frecuencia deseada) } - 1 = 15624
    	OCR0A = 156;

	// Setea el bit WGM01 (Waveform Generation Mode bit 01) en TCCR0A para configurar el Timer0 en modo CTC (Clear Timer on Compare Match)
	// En este modo, el Timer0 se reinicia a 0 cada vez que alcanza el valor en OCR0A (156)
	// Esto permite generar interrupciones periódicas con precisión y que el timer no cuente indefinidamente
    	TCCR0A |= (1 << WGM01);

	// Setea los bits CS02 y CS00 en TCCR0B para seleccionar un prescaler de 1024
	// El prescaler divide la frecuencia del reloj del sistema (16MHz) para que el Timer0 cuente a una velocidad más manejable
	// Con un prescaler de 1024, el Timer0 contará a 16MHz/1024 = 15625Hz
	// Esto significa que el Timer0 incrementará su valor en 1 cada 64 microsegundos
    	TCCR0B |= (1 << CS02) | (1 << CS00);

	// Setea el bit OCIE0A (Output Compare Match Interrupt Enable) en el registro Timer Interrupt Mask Register 0 (TIMSK0)
	// Esto habilita la interrupción que se genera cuando el Timer0 alcanza el valor en OCR0A
	// Cuando esto ocurre, se ejecuta la rutina de servicio de interrupción asociada (ISR(TIMER0_COMPA_vect))
	// Esta interrupción se utilizará para incrementar el cronómetro cada 10ms
    	TIMSK0 |= (1 << OCIE0A);
}

ISR(TIMER0_COMPA_vect) 
{
	static uint8_t cuenta = 0;
    	if (run) {
        	cuenta++;
        	if (cuenta >= 10) {   // 10 ms
            	contador++;
            	cuenta = 0;
        	}
    	}
} 

