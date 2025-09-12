/*
 Migraci�n a c�digo AVR del ejemplo Button de Arduino:
 http://www.arduino.cc/en/Tutorial/Button
 */

#include <avr/io.h>
#include <util/delay.h>


// variables
int buttonState = 0;         // variable para leer el estado del pulsador

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
		
		if(buttonState) { //Si el pulsador est� apretado
			//Apagar el led
			PORTB &= ~(1<<PB5);
		} else { 
			
			//Encender el led
			PORTB |= (1<<PB5);
		}

	}

}
