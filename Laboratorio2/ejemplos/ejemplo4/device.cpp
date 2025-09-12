
// Este device controla los eventos del deviceX, lo que no sabe, es que eventos ejecutar.

#include <Arduino.h>
#include <avr/interrupt.h>
#include "device.h"

void (*callback_fn)();

//#####################     ISR      ##################################
void isr()
{
  callback_fn();
}

//######################  Rutinas API #################################

void device_init(device_cfg *config)
{
  // Configurar el PIN como entrada con resistena de PULL UP
  pinMode(config->pin, INPUT_PULLUP);
  // Guarda el evento
  callback_fn = config->callback_fn; 
  // Habilita una interrupcion cuando encuentra un cambio en el PIN=2
  attachInterrupt(digitalPinToInterrupt(config->pin), isr, CHANGE) ; 
}
