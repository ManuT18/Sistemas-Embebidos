#include "teclado_driver.h"
#include "fnqueue.h"

// Valores de ADC para cada tecla (umbral)
static const uint16_t adc_key_val[NUM_KEYS] = {30, 250, 360, 535, 760};

// Estado del teclado
static int last_key = -1;
static int db_counter = 0; // contador anti-rebote
static int tecla_actual = -1;

// Callbacks registrados
static handler_t handlers[NUM_KEYS];
static int num_keys = NUM_KEYS;

// -------------------
// Función que interpreta el ADC y devuelve índice de tecla
// -------------------
static int get_key(uint16_t adc_value)
{
	for (int i = 0; i < num_keys; i++)
	{
		if (adc_value < adc_key_val[i])
		{
			return i;
		}
	}
	return -1; // ninguna tecla presionada
}

// -------------------
// Callback que el ADC llama cuando termina la conversión
// -------------------
static void keyboard_adc_callback(uint8_t channel, uint16_t adc_value)
{
	tecla_actual = get_key(adc_value);

	db_counter++;
	if (db_counter >= 8)
	{ // anti-rebote
		db_counter = 0;

		if (tecla_actual != last_key)
		{

			if (tecla_actual != -1 && handlers[tecla_actual].key_down)
			{
				handlers[tecla_actual].key_down();
			}

			if (last_key != -1 && handlers[last_key].key_up)
			{
				handlers[last_key].key_up();
			}

			last_key = tecla_actual;
		}
	}
}

// -------------------
// Inicialización del teclado (configura ADC)
// -------------------
void keyboard_init()
{
	adc_cfg_t adc_cfg;
	adc_cfg.channel = 0; // canal donde está conectado el teclado
	adc_cfg.callback = keyboard_adc_callback;

	adc_init(&adc_cfg);
}

// -------------------
// Registro de callbacks
// -------------------
void key_down_callback(void (*handler)(), int tecla)
{
	if (tecla >= 0 && tecla < NUM_KEYS)
	{
		handlers[tecla].key_down = handler;
	}
}

// void key_up_callback(void (*handler)(), int tecla) {
//     if (tecla >= 0 && tecla < NUM_KEYS) {
//         handlers[tecla].key_up = handler;
//     }
// }
