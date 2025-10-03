#include <avr/io.h>
#include <avr/interrupt.h>
#include "adc_driver.h"
#include "fnqueue.h"
#include "critical.h"

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


// ISR del timer para iniciar conversiones ADC cada 75 ms (LDR, canal 1)
ISR(TIMER1_COMPA_vect)
{
    if (!(ADCSRA & (1 << ADSC))) {  // Solo iniciar si no hay conversión en curso
        canal_actual = 1;
        ADMUX = (ADMUX & 0xF0) | (canal_actual & 0x0F);
        ADCSRA |= (1 << ADSC);
    }
}


// ISR del timer para iniciar conversiones ADC cada 10 ms (teclado, canal 0)
ISR(TIMER2_COMPA_vect)
{
    if (!(ADCSRA & (1 << ADSC))) {  // Solo iniciar si no hay conversión en curso
        canal_actual = 0;
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
}


bool adc_init(const adc_cfg_t *cfg)
{
    // Verificar canal válido
    if(cfg->channel >= ADC_MAX_CHANNELS) return false;

    // Guardar callback y marcar canal como activo
    adc_channels[cfg->channel] = cfg->callback;
    canal_activo[cfg->channel] = true;

    // Configuración global del ADC (solo la primera vez)
    if(!inicializado)
    {
        inicializado = true;

        // Referencia AVCC
        ADMUX = (1 << REFS0);

        // Prescaler 128 → 125 kHz para Arduino UNO
        ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

        // Configurar Timer1 para interrumpir cada 75 ms (LDR, canal 1)
        TCCR1A = 0;  // Normal mode
        TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);  // CTC, prescaler 1024
        OCR1A = 1156;  // (0.075 * 16000000 / 1024) - 1 ≈ 1156
        TIMSK1 = (1 << OCIE1A);  // Habilitar interrupción en compare match

        // Configurar Timer2 para interrumpir cada 10 ms (teclado, canal 0)
        TCCR2A = (1 << WGM21);  // CTC mode
        TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);  // Prescaler 1024
        OCR2A = 155;  // (0.01 * 16000000 / 1024) - 1 ≈ 155
        TIMSK2 = (1 << OCIE2A);  // Habilitar interrupción en compare match
    }
        
    return true;
}


static void adc_handler(void)
{
    volatile uint8_t canal_actual_aux = canal_actual;
    if(adc_channels[canal_actual_aux])
    {
        critical_begin();
        adc_channels[canal_actual_aux](canal_actual_aux, adc_values[canal_actual_aux]);
        critical_end();
    }
}