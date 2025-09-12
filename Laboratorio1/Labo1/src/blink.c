/*
 Migración a código AVR del ejemplo Blink de Arduino:
 http://www.arduino.cc/en/Tutorial/Blink
 */


#include <avr/io.h>
#include <util/delay.h>

int main()
{
	//Se inicializa el pin digital como salida.
	DDRB |= (1<<DDB5);

	while (1)
	{
		PORTB |= (1<<PB5);	//Enciende el Led
		_delay_ms(500);		//Espera medio segundo
		PORTB &= ~(1<<PB5); //Apaga el Led
		_delay_ms(500);		//Espera medio segundo
	}
}
