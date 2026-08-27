#ifndef ADC_DRIVER_H
#define ADC_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

#define ADC_MAX_CHANNELS 8

typedef void (*adc_callback_t)(uint8_t channel, uint16_t adc_value);

typedef struct {
    uint8_t channel;
    adc_callback_t callback;
} adc_cfg_t;

bool adc_init(const adc_cfg_t *cfg);

#endif
