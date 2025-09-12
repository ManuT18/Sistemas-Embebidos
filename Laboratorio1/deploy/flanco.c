/* 
 C�digo AVR de ejemplo para detectar un flanco
 Se utiliza el hardware del ejemplo Button de Arduino
  http://www.arduino.cc/en/Tutorial/Button
 */

#include <avr/io.h>
#include <util/delay.h>

// Variables:
int buttonState;       // the current reading from the input pin
int lastButtonState = 0;   // the previous reading from the input pin

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
