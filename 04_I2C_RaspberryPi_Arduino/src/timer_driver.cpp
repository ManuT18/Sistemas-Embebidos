#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer_driver.h"
#include "adc_driver.h"

// Inicializa Timer1
void timer_init(void)
{
	// Configurar Timer1 en CTC con prescaler 1024 para un tick base de ~5 ms
	// De esta forma podemos pedir teclado cada 2 ticks (~10 ms) y LDR cada 15 ticks (~75 ms)

	TCCR1A = 0;								 // Normal mode
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // CTC, prescaler 1024
	OCR1A = 77;								 // (0.005 * 16000000 / 1024) - 1 ≈ 77
	TIMSK1 = (1 << OCIE1A);						 // Habilitar interrupción en compare match
}

// Alternador de canales: cada vez que se dispara Timer1, alternamos entre canal 0 y 1
ISR(TIMER1_COMPA_vect)
{
	// Esta ISR se ejecuta cada ~5 ms (tick base).
	//  - cada 2 ticks (~10 ms) se solicita conversión del canal 0 (teclado)
	//  - cada 15 ticks (~75 ms) se solicita conversión LDR del canal 1
	static uint8_t ldr_next_channel = 1;
	static uint8_t tick_count = 0;

	tick_count++;

	// Teclado: cada 2 ticks -> ~10 ms
	if (tick_count == 2)
	{
		adc_start_conversion_channel(0);
	}

	// LDR: cada 15 ticks -> ~75 ms (canal 1)
	if (tick_count >= 15)
	{
		tick_count = 0;
		adc_start_conversion_channel(1);
	}
}
