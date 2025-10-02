// ldr.h
#ifndef LDR_H
#define LDR_H
#include <stdint.h>

// Variables para guardar los valores promedio y minimos y maximos historicos
extern float min_lux;
extern float max_lux;  
extern float lux_avg;

typedef void (*ldr_callback_t)(float lux);

typedef struct {
    uint8_t channel;          // Canal ADC donde está conectado el LDR
    ldr_callback_t callback;  // Función a llamar con el valor de lux
} ldr_cfg_t;

void ldr_init(const ldr_cfg_t *cfg);

#endif
