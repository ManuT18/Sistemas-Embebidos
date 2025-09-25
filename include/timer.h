#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <LiquidCrystal.h>

// Declaraciones de variables globales externas (si se necesitan acceder desde otros archivos)
extern volatile uint32_t contador;
extern volatile uint8_t run;

// Declaraciones de funciones públicas para manejo del timer y cronómetro
void TIMER_init(void);
void mostrarCronometro(LiquidCrystal &lcd);

#endif // TIMER_H
