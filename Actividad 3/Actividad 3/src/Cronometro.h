#ifndef CRONOMETRO_H
#define CRONOMETRO_H


#include <LiquidCrystal.h>


class Cronometro {
public:
Cronometro(LiquidCrystal &lcd);
void iniciar();
void pausar();
void reiniciar();
void actualizar(); // actualiza la LCD si flag_update
void incrementar(); // se llama desde ISR
bool estaCorriendo(); // devuelve true si el cronómetro está corriendo

private:
    LiquidCrystal &lcd;
    volatile unsigned long decimas; // sigue siendo volatile
    bool flag_update;               // ya no volatile
    bool running;                   // ya no volatile

};
#endif

