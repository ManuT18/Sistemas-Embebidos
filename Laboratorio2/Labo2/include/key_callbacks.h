// Punteros a funciones de callback (inicialmente NULL)
static void (*key_down_handler)(int tecla) = 0;
static void (*key_up_handler)(int tecla) = 0;

// Funciones para registrar callbacks
void key_down_callback(void (*handler)(int tecla)) {
    key_down_handler = handler;
}

void key_up_callback(void (*handler)(int tecla)) {
    key_up_handler = handler;
}
