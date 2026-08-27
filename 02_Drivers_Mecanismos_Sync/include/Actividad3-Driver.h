#ifndef ACTIVIDAD2_DRIVER_H
#define ACTIVIDAD2_DRIVER_H

void key_down_callback(void (*handler)(int tecla));
void key_up_callback(void (*handler)(int tecla));

void ADC_init(void);
void TIMER_init(void);

uint16_t get_key(unsigned int input);

void key_event_handler(void);

void incrementar();

extern volatile uint32_t contador;   // contador global (ticks de 10ms)
extern volatile uint8_t run;         // flag de ejecución (0 = detenido, 1 = corriendo)

void mostrarCronometro(LiquidCrystal &lcd);

#endif // ACTIVIDAD2_DRIVER_H