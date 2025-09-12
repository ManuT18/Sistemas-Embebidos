#include <avr/io.h>
#include <util/delay.h>

int buttonState;       
int lastButtonState = 0;   
int debounceDelay = 10;

#define F_CPU 16000000UL    // Frecuencia del reloj (16 MHz en Arduino UNO)
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

void UART_Init(unsigned int ubrr)
{
	// Configurar la velocidad del baud rate
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;

	// Habilitar recepcion y transmision
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	// Configurar el formato del frame: 8 bits de datos, 1 bit de parada
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void USART_SendString(const char* str) {
	while (*str) {
	    USART_Transmit(*str++);
	}
}

void USART_Transmit(unsigned char data)
{
	// Esperar hasta que el buffer de transmision este vacio
	while (!(UCSR0A & (1 << UDRE0)));
	// Colocar el dato en el buffer, lo que inicia la transmision
	UDR0 = data;
}

int main()
{
	UART_Init(MYUBRR);

	const char* mensaje = "Sistema Iniciado\r\n";
	USART_SendString(mensaje);

	DDRB |= (1<<DDB5);
	unsigned char mode = 0;

	while (1)
	{
		if (UCSR0A & (1 << RXC0)) {
			unsigned char input = UDR0; // Cambia el modo entre 0 y 5, segun lo que haya en el buffer
			mode = (input - '0') % 6; // Asegura que el modo este entre 0
			USART_SendString("Modo cambiado\r\n");

			switch (mode) {
				case 0:
					USART_SendString("Modo 0: LED apagado\r\n");
					break;
				case 1:
					USART_SendString("Modo 1: LED encendido\r\n");
					break;
				case 2:
					USART_SendString("Modo 2: Toggle LED cada 1000ms\r\n");
					break;
				case 3:
					USART_SendString("Modo 3: Toggle LED cada 500ms\r\n");
					break;
				case 4:
					USART_SendString("Modo 4: Toggle LED cada 250ms\r\n");
					break;
				case 5:
					USART_SendString("Modo 5: Toggle LED: 1/2 segundo encendido, 1 segundo apagado\r\n");
					break;
				default:
					break;
			}
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
