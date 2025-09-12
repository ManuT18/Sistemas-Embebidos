/* 
 C�digo AVR de ejemplo para realizar debounce por Software
 Si bien la estrategia de debounce difiere, el c�digo est� inspirado en el ejemplo Debounce 
 de Arduino, y el hardware es el utilizado en dicho ejemplo.
  http://www.arduino.cc/en/Tutorial/Debounce
 */

#include <avr/io.h>
#include <util/delay.h>

// Variables:
int buttonState;       // the current reading from the input pin
int lastButtonState = 0;   // the previous reading from the input pin
int debounceDelay = 50;    // the debounce time; increase if the output flickers

int main()
{
	//Inicializar el pin del pulsador como entrada
	DDRD &= ~(1<<DDD2);
	//Inicializar el pin del led como salida
	DDRB |= (1<<DDB5);

	while (1)
	{
		//Leer el estado del pulsador
		buttonState = PIND & (1<<PD2);

		//Si cambi� el estado del pulsador...
		if (buttonState != lastButtonState)
		{
			//Se espera un tiempo para evitar leer el ruido del rebote del pulsador
			_delay_ms(debounceDelay);

			//Se vuelve a leer el estado del pulsador
			buttonState = PIND & (1<<PD2);

			//Si el cambio se mantiene, se interpreta como evento de keydown/keyup v�lido
			if (buttonState != lastButtonState)
			{
				//El pulso es v�lido, se implementa la l�gica del sistema

				//Si no est� presionado el pulsador (se detect� un keyup)...
				if (!buttonState)
				{					
					PORTB ^= (1<<PB5); //Se modifica el estado del led (toggle v�a XOR)
				}

				//Si hubo cambios v�lidos actualizo el valor de lectura anterior
				lastButtonState = buttonState;
			}
		}
	}
}
