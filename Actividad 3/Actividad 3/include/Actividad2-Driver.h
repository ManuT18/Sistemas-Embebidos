#ifndef ACTIVIDAD2_DRIVER_H
#define ACTIVIDAD2_DRIVER_H

void key_down_callback(void (*handler)(int tecla));
void key_up_callback(void (*handler)(int tecla));
void ADC_init(void);
uint16_t get_key(unsigned int input);

#endif // ACTIVIDAD2_DRIVER_H