


#include "device.h"
#include <Arduino.h>
device_cfg deviceX;


// Funcion de callback
void toggleled1()
{
  PORTB ^= (1<<DDB5);
}

void setup() {

  DDRB |= (1<<DDB5);
  deviceX.pin = 2;
  deviceX.callback_fn =  toggleled1;
  device_init(&deviceX);

}

void loop() {
}
