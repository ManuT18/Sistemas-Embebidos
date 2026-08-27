#include <avr/io.h>
#include <util/delay.h>

int buttonState;       
int lastButtonState = 0;   
int debounceDelay = 2;
int mode = 0;

int main()
{
	DDRD &= ~(1<<DDD2);
	DDRB |= (1<<DDB5);

	while (1)
	{
		buttonState = PIND & (1<<PD2);

		if (buttonState != lastButtonState)
		{
			_delay_ms(debounceDelay);
			buttonState = PIND & (1<<PD2);

			if (!buttonState)
			{					
				mode = (mode+1) % 6; // Cambia el modo entre 0 y 4
			}

			lastButtonState = buttonState;
		}

		switch (mode)
		{
		case 0:
			PORTB &= ~(1<<PB5); // LED apagado
			break;
		case 1:
			PORTB |= (1<<PB5); // LED encendido
			break;
		case 2:
			PORTB ^= (1<<PB5); // Toggle LED cada 1000ms
			_delay_ms(1000);
			break;
		case 3:
			PORTB ^= (1<<PB5); // Toggle LED cada 500ms
			_delay_ms(500);
			break;
		case 4:
			PORTB ^= (1<<PB5); // Toggle LED cada 250ms
			_delay_ms(250);
			break;
		case 5:
			PORTB ^= (1<<PB5); // Toggle LED: 1/2 segundo encendido, 1 segundo apagado
			_delay_ms(500);
			PORTB ^= (1<<PB5);
			_delay_ms(1000);
			break;
		default:
			break;
		}
	}
}
