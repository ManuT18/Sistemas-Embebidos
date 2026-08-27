#include <pigpio.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <vector>

using namespace std;

// Protocolo Arduino
const uint8_t SOF = 0x7E;
const uint8_t EOF_B = 0x7F;
const uint8_t ESC = 0x7D;

const uint8_t OBTENER_LUX = 0x01;
const uint8_t OBTENER_MAX = 0x02;
const uint8_t OBTENER_MIN = 0x03;
const uint8_t OBTENER_PROM = 0x04;
const uint8_t OBTENER_TODO = 0x05;

// Función para enviar un comando simple (sin payload)
void send_command(int handle, uint8_t cmd)
{
	char buf[4];
	buf[0] = SOF;
	buf[1] = sizeof(cmd); // length = 1 byte de payload (solo TYPE)
	buf[2] = cmd;	// TYPE = comando
	buf[3] = EOF_B;
	i2cWriteDevice(handle, buf, 4);
}

// Función para leer frame completo
vector<uint8_t> read_frame(int handle)
{
	vector<uint8_t> frame;
	char byte;
	bool in_frame = false;

	// Buffer de lectura por bloque
	unsigned char buf[64];

	// timeout de lectura para evitar bloqueo indefinido
	const int timeout_ms = 2000; // 2 segundos
	int waited_ms = 0;

	while (waited_ms < timeout_ms)
	{
		int r = i2cReadDevice(handle, (char*)buf, sizeof(buf));
		if (r < 0) {
			cerr << "i2cReadDevice error: " << r << endl;
			break; // error en I2C
		}
        
		// nota: suprimir impresión de cantidad de bytes leidos para reducir ruido
		usleep(500);

		// Procesar todos los bytes leídos en esta transacción
		for (int k = 0; k < r; k++) {
			
			uint8_t b = static_cast<uint8_t>(buf[k]);
			
			if (!in_frame) {
				
				if (b == SOF) {
					frame.clear();
					frame.push_back(b);
					in_frame = true;
				}

			} else {
				
				frame.push_back(b);
				if (b == EOF_B) {
					if (!frame[k-1] == ESC){
						// fin de frame detectado
						goto done_reading;
					}
				}

			}
		}
	}

	done_reading:

	// Frame leido (se suprime volcado por defecto)

	return frame;
}

// Parsear payload ASCII desde frame
string parse_payload(const vector<uint8_t> &frame)
{
	if (frame.size() < 4)
		return ""; // SOF + LEN + TYPE + EOF mínimo
	
	string payload;

	for (size_t i = 3; i < frame.size() - 1; ++i)
	{ // desde payload hasta EOF-1
		if (frame[i] == ESC && i + 1 < frame.size())
		{
			payload.push_back(frame[i + 1]); // manejo escape
			i++;
		}
		else
		{
			payload.push_back(frame[i]);
		}
	}

	return payload;
}

// Opcional: parsear el tipo de respuesta para diferenciar Lux, Max, Min, Prom
uint8_t parse_type(const vector<uint8_t> &frame)
{
	if (frame.size() < 4)
		return 0;
	
	return frame[2]; // TYPE está en la posición 2
}

int main()
{
	if (gpioInitialise() < 0)
	{
		cerr << "Error inicializando pigpio" << endl;
		return 1;
	}

	int i2c_handle = i2cOpen(1, 0x08, 0);
	if (i2c_handle < 0)
	{
		cerr << "Error abriendo I2C" << endl;
		gpioTerminate();
		return 1;
	}

	uint16_t contador = 0;
	int fail_count = 0; // contador de fallos consecutivos de lectura

	while (true)
	{
		// Pedir al usuario qué comando enviar: 1..5. 'q' para salir.
		cout << "Ingrese comando (1=OBTENER_LUX,2=MAX,3=MIN,4=PROM,5=TODO) o 'q' para salir: ";
		string line;
		if (!std::getline(cin, line)) {
			// EOF en stdin -> salir
			break;
		}

		if (!line.empty() && (line[0] == 'q' || line[0] == 'Q')) {
			cout << "Saliendo...\n";
			break;
		}

		// Intentar parsear un número
		uint8_t val;
		try
		{
			val = stoi(line);
		}
		catch(const std::exception& e)
		{
			val = 0;
			goto outofcatch;
		}

		outofcatch:
		
		if (val < 1 || val > 5) {
			cout << "Valor fuera de rango. Use 1..5.\n";
		}

		// 1) enviar comando
		send_command(i2c_handle, val);
		// Dar tiempo al Arduino para procesar el mensaje y preparar la respuesta
		usleep(50000); // 50 ms

		// 2) leer frame completo
		vector<uint8_t> frame = read_frame(i2c_handle);

		if (frame.empty()) {
			fail_count++;
			cerr << "read_frame returned empty (fail_count=" << fail_count << ")" << endl;
		} else {
			fail_count = 0;
		}
		//cout << "sali de read_frame\n";

		// Si hubo varios fallos seguidos, intentar reabrir el handle I2C
		if (fail_count >= 3) {
			cerr << "Múltiples fallos I2C, intentando reabrir el bus..." << endl;
			i2cClose(i2c_handle);
			usleep(10000);
			i2c_handle = i2cOpen(1, 0x08, 0);
			if (i2c_handle < 0) {
				cerr << "Error reabriendo I2C: " << i2c_handle << ", esperando y reintentando" << endl;
				usleep(500000);
			} else {
				cerr << "I2C reabierto correctamente (handle=" << i2c_handle << ")" << endl;
				fail_count = 0;
			}
		}

		// 3) parsear payload
		string lux_str = parse_payload(frame);
		//cout << "sali de parse_payload\n";

		// 4) parsear tipo
		uint8_t type = parse_type(frame);
		//cout << "tipo: " << type << endl;
		switch (type)
		{
		case 0x81:
			cout << "RESPONDER_LUX: " << lux_str << endl;
			break;
		case 0x82:
			cout << "RESPONDER_MAX: " << lux_str << endl;
			break;
		case 0x83:
			cout << "RESPONDER_MIN: " << lux_str << endl;
			break;
		case 0x84:
			cout << "RESPONDER_PROM: " << lux_str << endl;
			break;
		case 0x85:
			cout << "RESPONDER_TODO: " << lux_str << endl;
			break;
		default:
			cout << "RESPONDER_DESCONOCIDO: " << lux_str << endl;
			break;
		}

		contador++;
		// separador de linea
		cout << endl;

		usleep(50000); 
		// cout << "\nNEW\n\n";
	}

	i2cClose(i2c_handle);
	gpioTerminate();
	return 0;
}
