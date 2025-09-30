#include "ldr.h"
#include "adc_driver.h"  
#include <Arduino.h>
#include <stdio.h>

// Callback del usuario
static ldr_callback_t ldr_user_callback;

// Tabla de resistencias y lux para interpolación
// Se usó la tabla ldr_R en base a mediciones realizadas con el LDR y una resistencia fija de 1kΩ
// La tabla ldr_lux es la dada en el enunciado del laboratorio
static const float ldr_R[] = {25000, 12589, 6310, 3162, 1585, 794, 398, 200, 100};
static const float ldr_lux[] = {0.5, 1, 3, 6, 10, 15, 35, 80, 100}; 


static float resistance_to_lux(float R)
{
    int i;

    // Caso fuera de rango
    if (R >= ldr_R[0]) return ldr_lux[0];
    if (R <= ldr_R[8]) return ldr_lux[8];

    
    for (i = 0; i < 8; i++) {
        if (R <= ldr_R[i] && R >= ldr_R[i+1]) {
     
            float lux = ldr_lux[i] + (R - ldr_R[i]) * (ldr_lux[i+1] - ldr_lux[i]) / (ldr_R[i+1] - ldr_R[i]);
            return lux;

        }
    }
    return 0; 
}


static void ldr_adc_callback(uint8_t channel, uint16_t adc_value)
{
    float Vadc = (adc_value / 1023.0f) * 5.0f;
    float R_fixed = 1000.0f;
    float Rldr = R_fixed * (5.0f / Vadc - 1.0f);

    float lux = resistance_to_lux(Rldr);

    if (ldr_user_callback) {
        ldr_user_callback(lux);
    }
}

void ldr_init(const ldr_cfg_t *cfg)
{
    ldr_user_callback = cfg->callback;

    adc_cfg_t adc_cfg;
    adc_cfg.channel = cfg->channel;
    adc_cfg.callback = ldr_adc_callback;

    adc_init(&adc_cfg);
}
