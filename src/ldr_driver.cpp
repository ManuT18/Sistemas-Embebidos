#include "ldr_driver.h"
#include "adc_driver.h"  
#include <Arduino.h>
#include <stdio.h>

// Callback del usuario
static ldr_callback_t ldr_user_callback;

// Tabla de resistencias y lux para interpolación
static const float ldr_R[] = {92000, 41000, 24000, 16000, 10000, 7000, 5000, 1000, 500};
static const float ldr_lux[] = {0.5, 1, 3, 6, 10, 15, 35, 80, 100}; 

// Variables para guardar los valores promedio y minimos y maximos historicos
static float min_lux = -1.0f;
static float max_lux = 0.0f;
static float lux_avg = 0.0f;
static float lux_percentage = 0.0f;  

float get_min_lux() { return min_lux; }
float get_max_lux() { return max_lux; }
float get_lux_avg() { return lux_avg; }
float get_lux_percentage() { return lux_percentage; }

// Definición del arreglo circular para almacenar los últimos 200 valores de lux
static float lux_buffer[200];
static int buffer_index = 0;
static int buffer_count = 0;


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


void update_min_max_from_buffer() {
    min_lux = lux_buffer[0];
    max_lux = lux_buffer[0];

    for (int i = 1; i < buffer_count; i++) {
        if (lux_buffer[i] < min_lux) min_lux = lux_buffer[i];
        if (lux_buffer[i] > max_lux) max_lux = lux_buffer[i];
    }

}


void store_lux_on_buffer(float lux) {
    // Almacenar el valor de lux en el buffer circular
    lux_buffer[buffer_index] = lux;
    buffer_index = (buffer_index + 1) % 200;
    if (buffer_count < 200) {
        buffer_count++;
    }

    // Actualizar min y max del buffer
    update_min_max_from_buffer();
}


void recalculate_lux_avg() {
    float nuevo_valor = lux_buffer[(buffer_index - 1 + 200) % 200];
    float sum;

    if (buffer_count < 200) {
        // Buffer no lleno
        sum = lux_avg * (buffer_count - 1) + nuevo_valor;
    } else {
        // Buffer lleno: restar el valor saliente y agregar el nuevo
        float valor_saliente = lux_buffer[buffer_index];
        sum = lux_avg * buffer_count - valor_saliente + nuevo_valor;
    }

    lux_avg = sum / buffer_count;
}


static void ldr_adc_callback(uint8_t channel, uint16_t adc_value)
{
    if (channel == 1) { // Asegurarse de que es el canal correcto
    
        float Vadc = (adc_value / 1023.0f) * 5.0f;
        float R_fixed = 1000.0f;
        float Rldr = R_fixed * (5.0f / Vadc - 1.0f);

        float lux = resistance_to_lux(Rldr);

        //Serial.println("pase resistance_to_lux ");

        store_lux_on_buffer(lux);

        //Serial.println("pase store_lux_on_buffer ");

        recalculate_lux_avg();

        // Calcular porcentaje del rango min-max
        if (max_lux > min_lux) {
            lux_percentage = ((lux - min_lux) / (max_lux - min_lux)) * 100.0f;
        } else {
            lux_percentage = 0.0f;  // Si min == max, porcentaje 0
        }

      //  Serial.println("pase recalculate_lux_avg ");

        // Serial.print("  Rldr: "); Serial.print(Rldr); 
        // Serial.print("  Lux: "); Serial.print(lux);
         //Serial.print("      Max: "); Serial.print(max_lux);
         //Serial.print("      Min: "); Serial.print(min_lux);
         //Serial.print("      Avg: "); Serial.println(lux_avg);

        if (ldr_user_callback) {
            ldr_user_callback(lux);
        }

    }
}


void ldr_init(const ldr_cfg_t *cfg)
{
    ldr_user_callback = cfg->callback;

    adc_cfg_t adc_cfg;
    adc_cfg.channel = cfg->channel;
    adc_cfg.callback = ldr_adc_callback;

    if (!adc_init(&adc_cfg)) {
        Serial.println("Error al ejecutar inicializacion ADC para LDR");
    }
}
