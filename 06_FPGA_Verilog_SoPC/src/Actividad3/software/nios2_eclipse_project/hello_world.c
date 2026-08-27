#include "alt_types.h"
#include "altera_avalon_pio_regs.h"
#include "sys/alt_irq.h"
#include "system.h"
#include <stdio.h>
#include "io.h"

int main()
{
	printf("nios compilado\n");
	while (1)
		IOWR_ALTERA_AVALON_PIO_DATA(PIO_LEDS_BASE, IORD_ALTERA_AVALON_PIO_DATA(PIO_SWITCHES_BASE));
}
