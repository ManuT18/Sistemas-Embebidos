#ifndef COMM_PROTOCOL_H
#define COMM_PROTOCOL_H

#include <stdint.h>
// comm_protocol.h
extern float lux_actual;  // Esto dice: "existe en otro lado" y efectivamente existe en el main.cpp

// Inicializa el módulo de protocolo de comunicación (registra callbacks I2C)
void comm_init(void);

// Handler para ser llamado desde Wire.onReceive
void comm_on_receive(int howMany);

// Handler para ser llamado desde Wire.onRequest
void comm_on_request();

// Procesa datos en contexto no-ISR (puede ser encolado con fnqueue)
void comm_process(void);

#endif // COMM_PROTOCOL_H
