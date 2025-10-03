#include <stdbool.h>
#include <stdint.h>
#include <Arduino.h>
#include <LiquidCrystal.h>
#include "fnqueue.h"
#include "critical.h"




//########################################################################################
//###																  ###
//###					DEFINICION DE VARIABLES							  ###
//###																  ###
//########################################################################################

volatile uint32_t contador = 0;  
volatile uint8_t run = 0;



//########################################################################################
//###																  ###
//###					DEFINICION DE FUNCIONES							  ###
//###																  ###
//########################################################################################

ISR(TIMER0_COMPA_vect);
void incrementar();
void TIMER_init(void);



//########################################################################################
//###																  ###
//###				CONFIGURACIONES Y FUNCIONES DEL TIMER					  ###
//###																  ###
//########################################################################################

void TIMER_init(void)
{
	// Configurar Timer0 para que genere una interrupción cada 100ms

	// Limpia cualquier configuración previa de los registros Timer Counter Control Register 2 A y B
	// El registro A controla aspectos como el modo de onda y las salidas de comparación
	// El registro B controla el prescaler, modo de generación de ondas y fuente de reloj
    	TCCR2A = 0;
    	TCCR2B = 0;

	// Asigna el valor de comparación de 156 = 15624/100 al registro Out Compare Register 2 A
	// El valor 15624 se obtiene de la fórmula:
	// { 16MHz / 1024 (prescaler) / 1Hz (frecuencia deseada) } - 1 = 15624
    	OCR2A = 156;

	// Setea el bit WGM21 (Waveform Generation Mode bit 21) en TCCR2A para configurar el Timer2 en modo CTC (Clear Timer on Compare Match)
	// En este modo, el Timer2 se reinicia a 0 cada vez que alcanza el valor en OCR2A (156)
	// Esto permite generar interrupciones periódicas con precisión y que el timer no cuente indefinidamente
    	TCCR2A |= (1 << WGM21);

	// Setea los bits CS22 y CS20 en TCCR2B para seleccionar un prescaler de 1024
	// El prescaler divide la frecuencia del reloj del sistema (16MHz) para que el Timer2 cuente a una velocidad más manejable
	// Con un prescaler de 1024, el Timer2 contará a 16MHz/1024 = 15625Hz
	// Esto significa que el Timer2 incrementará su valor en 1 cada 64 microsegundos
    	TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);

	// Setea el bit OCIE2A (Output Compare Match Interrupt Enable) en el registro Timer Interrupt Mask Register 2 (TIMSK2)
	// Esto habilita la interrupción que se genera cuando el Timer2 alcanza el valor en OCR2A
	// Cuando esto ocurre, se ejecuta la rutina de servicio de interrupción asociada (ISR(TIMER2_COMPA_vect))
	// Esta interrupción se utilizará para incrementar el cronómetro cada 10ms
    	TIMSK2 |= (1 << OCIE2A);
}

ISR(TIMER2_COMPA_vect) 
{
	static uint8_t cuenta = 0;
    	if (run) {
        	cuenta++;
        	if (cuenta >= 10) {   // 10 ms
            	contador++;            	
			cuenta = 0;
        	}
    	}
} 

void mostrarCronometro(LiquidCrystal &lcd) {
	
	critical_begin();
	uint32_t totalDecimas = contador;
	critical_end();

	uint32_t minutos = totalDecimas / 600;
	uint32_t segundos = (totalDecimas / 10) % 60;
	uint32_t decimas = totalDecimas % 10;
 
	// Reiniciar contador si llega a 60.60.10
	if (minutos == 60 && segundos == 60 && decimas == 10) {

		critical_begin();
		contador = 0; 
		critical_end();
		
		minutos = 0;
		segundos = 0;
		decimas = 0;
	}

	char buffer[9];
	sprintf(buffer, "%01lu.%02lu.%1lu", minutos, segundos, decimas);  
 
	// Escribir cronómetro
	lcd.setCursor(0,1);
	lcd.print(buffer);
 }