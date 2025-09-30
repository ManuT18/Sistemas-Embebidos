// ldr.h
#ifndef LDR_H
#define LDR_H
#include <stdint.h>

typedef void (*ldr_callback_t)(float lux);

typedef struct {
    uint8_t channel;          // Canal ADC donde está conectado el LDR
    ldr_callback_t callback;  // Función a llamar con el valor de lux
} ldr_cfg_t;

void ldr_init(const ldr_cfg_t *cfg);

#endif
