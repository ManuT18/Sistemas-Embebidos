#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h> 
//####################### Estructuras de control #####################
typedef struct
{
  uint8_t param_a;
  uint8_t pin;
  void   (*callback_fn)();
} device_cfg;

//#####################     Rutinas API    ###############################
void device_init(device_cfg *device);

#endif
