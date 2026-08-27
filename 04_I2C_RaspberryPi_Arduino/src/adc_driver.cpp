#include <avr/io.h>
#include <avr/interrupt.h>
#include "adc_driver.h"
#include "fnqueue.h"
#include "critical.h"
#include "timer_driver.h"

// Arreglo de callbacks por canal
static adc_callback_t adc_channels[ADC_MAX_CHANNELS];

// Últimos valores leídos
static volatile uint16_t adc_values[ADC_MAX_CHANNELS];

// Canal actual en conversión
static uint8_t canal_actual = 0;

// Máscara de canales activos
static bool canal_activo[ADC_MAX_CHANNELS];

// Handler encolado
static void adc_handler(void);

// Variable para evitar inicialización múltiple
static bool inicializado = false;

ISR(ADC_vect)
{
	// Leer valor ADC
	adc_values[canal_actual] = ADC;

	// Encolar handler fuera de ISR
	fnqueue_add(adc_handler);
}

bool adc_init(const adc_cfg_t *cfg)
{
	// Verificar canal válido
	if (cfg->channel >= ADC_MAX_CHANNELS)
		return false;

	// Guardar callback y marcar canal como activo
	adc_channels[cfg->channel] = cfg->callback;
	canal_activo[cfg->channel] = true;

	// Configuración global del ADC (solo la primera vez)
	if (!inicializado)
	{
		inicializado = true;

		// Referencia AVCC
		ADMUX = (1 << REFS0);

		// Prescaler 128 → 125 kHz para Arduino UNO
		ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

		// Inicializar timers (moved to timer_driver)
		timer_init();
	}

	return true;
}

static void adc_handler(void)
{
	volatile uint8_t canal_actual_aux = canal_actual;
	if (adc_channels[canal_actual_aux])
	{
		critical_begin();
		adc_channels[canal_actual_aux](canal_actual_aux, adc_values[canal_actual_aux]);
		critical_end();
	}
}

// Permite que otros módulos (ej. timer_driver) soliciten iniciar conversión en un canal
void adc_start_conversion_channel(uint8_t channel)
{
	if (channel >= ADC_MAX_CHANNELS)
		return;

	// Solo iniciar si no hay conversión en curso
	if (!(ADCSRA & (1 << ADSC)))
	{
		canal_actual = channel;
		ADMUX = (ADMUX & 0xF0) | (canal_actual & 0x0F);
		ADCSRA |= (1 << ADSC);
	}
}