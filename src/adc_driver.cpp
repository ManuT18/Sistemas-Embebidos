#include <avr/io.h>
#include <avr/interrupt.h>
#include "adc_driver.h"
#include "fnqueue.h"

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

void adc_init(const adc_cfg_t *cfg)
{
    if(cfg->channel >= ADC_MAX_CHANNELS) return;

    // Guardar callback y marcar canal como activo
    adc_channels[cfg->channel] = cfg->callback;
    canal_activo[cfg->channel] = true;

    // Configuración global del ADC (solo la primera vez)
    static bool inicializado = false;
    if(!inicializado)
    {
        inicializado = true;

        // Referencia AVCC
        ADMUX = (1 << REFS0);

        // Prescaler 128 → 125 kHz para Arduino UNO
        ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

        // Iniciar primera conversión en canal 0
        ADMUX = (ADMUX & 0xF0) | (canal_actual & 0x0F);
        ADCSRA |= (1 << ADSC);
    }
}

ISR(ADC_vect)
{
    // Leer valor ADC
    adc_values[canal_actual] = ADC;

    // Encolar handler fuera de ISR
    fnqueue_add(adc_handler);

    // Round-robin: buscar próximo canal activo
    uint8_t siguiente = canal_actual;
    for(uint8_t i=0; i<ADC_MAX_CHANNELS; i++)
    {
        siguiente = (siguiente + 1) % ADC_MAX_CHANNELS;
        if(canal_activo[siguiente])
        {
            canal_actual = siguiente;
            break;
        }
    }

    // Iniciar conversión en próximo canal
    ADMUX = (ADMUX & 0xF0) | (canal_actual & 0x0F);
    ADCSRA |= (1 << ADSC);
}

static void adc_handler(void)
{
    volatile uint8_t canal_actual_aux = canal_actual;
    if(adc_channels[canal_actual_aux])
    {
        adc_channels[canal_actual_aux](canal_actual_aux, adc_values[canal_actual_aux]);
    }
}
