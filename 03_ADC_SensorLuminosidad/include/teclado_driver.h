#ifndef TECLADO_H
#define TECLADO_H

#include <stdint.h>
#include "adc_driver.h"

// Máximo número de teclas
#define NUM_KEYS 5

typedef struct {
    void (*key_down)();   // Callback cuando se presiona la tecla
    void (*key_up)();     // Callback cuando se suelta la tecla
} handler_t;

// Inicialización del teclado
void keyboard_init();

// Registro de callbacks
void key_down_callback(void (*handler)(), int key);
void key_up_callback(void (*handler)(), int key);

#endif
