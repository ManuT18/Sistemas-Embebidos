/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 * Ejemplo minimal de trabajo con Timers e ISRs.

 *
 * Si bien se puede configurar la HAL para definir un system clock y registrar
 * alarmas (callbacks) en el mismo, vamos a trabajar con ISR a menor nivel de
 * abstracción.
 *
 */

#include <stdio.h>
#include <system.h>
#include <sys/alt_irq.h>
#include <altera_avalon_pio_regs.h>
#include <altera_avalon_timer_regs.h>

//Variable de conteo
volatile unsigned int counter;
volatile int new_value = 0;

static void timerISR(void* context)
{
	//Incrementa el contador en fcn de la base temporal del timer
	counter++;
    new_value = 1;

	//Muestra el contador por los leds
	IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, counter);

	//Limpia el flag de interrupción (evita la ejecución múltiple de la ISR ante la misma IRQ)
	IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0);
}

int main()
{
  printf("Hello from Nios II!\n");

  //Inicializa el Timer escribiendo su registro de control.
  IOWR_ALTERA_AVALON_TIMER_CONTROL (TIMER_0_BASE,
              ALTERA_AVALON_TIMER_CONTROL_ITO_MSK  |
              ALTERA_AVALON_TIMER_CONTROL_CONT_MSK |
              ALTERA_AVALON_TIMER_CONTROL_START_MSK);


  //Registra la ISR (callback) del Timer
  int regStatus = alt_ic_isr_register(TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID,
		  	  	  	  TIMER_0_IRQ,
		  	  	  	  &timerISR, 0x0, 0x0);

  //Ciclo principal vacío a la espera de interrupciones
  while (1)
    if (new_value)
        {
            //Monitoreo del valor a través de la consola (JTAG UART - sdout)
            printf("Counter value = %d\n", counter);
            new_value = 0;
        }

  return 0;
}
