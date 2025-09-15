#ifndef ACTIVIDAD2_DRIVER_H
#define ACTIVIDAD2_DRIVER_H

void key_down_callback(void (*handler)(int tecla));
void key_up_callback(void (*handler)(int tecla));
void ADC_init(void);
void TIMER_init(void);
uint16_t get_key(unsigned int input);
void key_event_handler(void);

#endif // ACTIVIDAD2_DRIVER_H