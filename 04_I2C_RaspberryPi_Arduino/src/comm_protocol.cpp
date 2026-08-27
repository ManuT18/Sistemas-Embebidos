#include "comm_protocol.h"
#include "fnqueue.h"
#include "ldr_driver.h"
#include <Wire.h>
#include <Arduino.h>
#include <string.h>

// Protocolo constants
static const uint8_t SOF = 0x7E;
static const uint8_t EOF_B = 0x7F;
static const uint8_t ESC = 0x7D;

// Message types
static const uint8_t OBTENER_LUX = 0x01;
static const uint8_t OBTENER_MAX = 0x02;
static const uint8_t OBTENER_MIN = 0x03;
static const uint8_t OBTENER_PROM = 0x04;
static const uint8_t OBTENER_TODO = 0x05;

static const uint8_t RESPONDER_LUX = 0x81;
static const uint8_t RESPONDER_MAX = 0x82;
static const uint8_t RESPONDER_MIN = 0x83;
static const uint8_t RESPONDER_PROM = 0x84;
static const uint8_t RESPONDER_TODO = 0x85;

// Buffers y estado
static uint8_t rx_buf[64];
static size_t rx_len = 0;

static uint8_t response_buffer[128];
static size_t response_len = 0;

// // Prebuilt BUSY frame to return when no response is ready
// static uint8_t busy_frame[32];
// static size_t busy_frame_len = 0;


static void build_frame_append(uint8_t *out, size_t &out_idx, uint8_t b)
{
	if (b == SOF || b == EOF_B || b == ESC)
	{
		out[out_idx++] = ESC;
		out[out_idx++] = b;
	}
	else
	{
		out[out_idx++] = b;
	}
}

static size_t build_frame(uint8_t *out, uint8_t type, const uint8_t *payload, size_t payload_len)
{
    size_t idx = 0;
    out[idx++] = SOF;                 				// (1) Inicio de trama
    size_t len_idx = idx++;           				// (2) Reservo espacio para LEN

    build_frame_append(out, idx, type); 			// (3) Agrego el TYPE con manejo de escape

    for (size_t i = 0; i < payload_len; ++i)
        build_frame_append(out, idx, payload[i]);	 	// (4) Agrego cada byte del payload

    out[idx++] = EOF_B;               				// (5) Fin de trama

    out[len_idx] = (uint8_t)(idx - 1 - len_idx); 		// (6) Calculo LEN

    return idx; 								// Devuelvo la cantidad total de bytes del frame
}


// Este parseo solo lo hacemos para obtener el type
static uint8_t parse_type(const uint8_t *buf, size_t len)
{
	if (len < 4)
		return 0; 							// Me aseguro que tenga al menos SOF, LEN, TYPE, EOF
	if (buf[0] != SOF)
		return 0; 							// También me aseguro de que el primer byte sea SOF
	
	
	
	bool esc = false;
	for (size_t i = 2; i < len; i++) 				// Arranco en el byte del TYPE porque SOF y LEN ya los chequeé
	{
		uint8_t b = buf[i];
		if (esc)
			return b;
		if (b == ESC)
		{
			esc = true;
			continue;
		}
		return b;
	}

	return 0;
}



void comm_process(void)
{
	delay(500);

	// Serial.println("Procesando comm");
	
	uint8_t type = parse_type(rx_buf, rx_len);
	rx_len = 0; 								

	// Serial.print("parsetype paso y dio: ");
	// Serial.println(type);

	// Serial.println("sin errores");

	// if (type == OBTENER_LUX){
	// 	Serial.println("CORRECTO");
	// }

	char payload[64];
	size_t payload_size = 0;
	switch (type)
	{
		case OBTENER_LUX:
			dtostrf(lux_actual, 6, 2, payload);
			payload_size = strlen(payload);
			response_len = build_frame(response_buffer, RESPONDER_LUX, (const uint8_t *)payload, payload_size);
			break;

		case OBTENER_MAX:
			dtostrf(get_max_lux(), 6, 2, payload);
			payload_size = strlen(payload);
			response_len = build_frame(response_buffer, RESPONDER_MAX, (const uint8_t *)payload, payload_size);
			break;
		
		case OBTENER_MIN:
			dtostrf(get_min_lux(), 6, 2, payload);
			payload_size = strlen(payload);
			response_len = build_frame(response_buffer, RESPONDER_MIN, (const uint8_t *)payload, payload_size);
			break;

		case OBTENER_PROM:
			dtostrf(get_lux_avg(), 6, 2, payload);
			payload_size = strlen(payload);
			response_len = build_frame(response_buffer, RESPONDER_PROM, (const uint8_t *)payload, payload_size);
			break;

		case OBTENER_TODO:
			char a[12], mi[12], ma[12], p[12];
			dtostrf(lux_actual, 6, 2, a);
			dtostrf(get_min_lux(), 6, 2, mi);
			dtostrf(get_max_lux(), 6, 2, ma);
			dtostrf(get_lux_avg(), 6, 2, p);
			snprintf(payload, sizeof(payload), "%s;%s;%s;%s", a, mi, ma, p);
			payload_size = strlen(payload);
			response_len = build_frame(response_buffer, RESPONDER_TODO, (const uint8_t *)payload, payload_size);
			break;

		default:
			response_len = build_frame(response_buffer, 0xFF, (const uint8_t *)"UNK", 3);
			break;
	}
	// Serial.println("Respuesta lista");
}

// Wire onReceive: minimo, copia los bytes a rx_buf y encola el procesado
// el howmany es la cantidad de bytes recibidos y lo hace la misma libreria wire
void comm_on_receive(int howMany)
{
	//Serial.println("Me llego un mje");
	size_t i = 0;
	while (Wire.available() && i < sizeof(rx_buf))
	{
		rx_buf[i++] = Wire.read();
	}
	rx_len = i;
	
	fnqueue_add(comm_process);
}

// void imprimir_trama_busy()
// {
// 	Serial.print("Enviando BUSY: ");
// 	for (size_t i = 0; i < busy_frame_len; ++i)
// 	{
// 		Serial.print(busy_frame[i]);
// 		Serial.print(" ");
// 	}
// 	Serial.println();
// }

uint16_t contador_mensajes = 0;
void printmensajeenviado()
{
	Serial.print("Mensaje enviado: ");
	Serial.println(contador_mensajes++);
}

void comm_on_request()
{
	if (response_len > 0)
	{
		Wire.write(response_buffer, response_len);
		response_len = 0;

		fnqueue_add(printmensajeenviado);

		return;
	}

	// // No hay respuesta lista -> responder con BUSY (preconstruido)
	// fnqueue_add(imprimir_trama_busy);
	// Wire.write(busy_frame, busy_frame_len);
}

void comm_init(void)
{
	Wire.onReceive(comm_on_receive);
	Wire.onRequest(comm_on_request);

	// // Construir el frame BUSY (TYPE = 0xFF, payload = "BUSY") para uso en ISR
	// const uint8_t busy_payload[] = {'B', 'U', 'S', 'Y'};
	// busy_frame_len = build_frame(busy_frame, 0xFF, busy_payload, sizeof(busy_payload));

}
