#include <stdbool.h>
#include <stdint.h>
#include "Arduino.h"
#include <LiquidCrystal.h>
#include "fnqueue.h"
#include "critical.h"
#include "key_callbacks.h"

const uint8_t numRows = 2;
const uint8_t numCols = 16;
volatile uint16_t adc_value = 0;
uint16_t adc_key_val[5] ={30, 150, 360, 535, 760 };
uint8_t NUM_KEYS = 5;
uint16_t adc_key_in;
uint16_t key=-1;
uint16_t oldkey=-1;
int tecla;

// Initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Key message
char msgs[5][17] = 
{
    " Right Key:  OK ", 
    " Up Key:     OK ", 
    " Down Key:   OK ", 
    " Left Key:   OK ", 
    " Select Key: OK "
};


uint16_t get_key(unsigned int input);
void ADC_init(void);
ISR(ADC_vect);
void onKeyDown(int tecla);
void onKeyUp(int tecla);


void setup() 
{
    lcd.begin(numCols,numRows);
    analogWrite(10, 100); // Controla intensidad backlight
    lcd.print("ADC con ISR");
    delay(1000);

    fnqueue_init();
    ADC_init();
    sei();  // habilitar interrupciones globales

    key_down_callback(onKeyDown);
    key_up_callback(onKeyUp);
}

void loop()
{
    // Ejecuta funciones encoladas
    fnqueue_run();

    for (int i = 0; i < 255 ; i++) {
        analogWrite(10, i);
        delay(10);
    }
    lcd.setCursor(0, 1);      //line=1, x=0
    lcd.print("ciclando");
    delay(300);
    lcd.setCursor(8, 1);      //line=1, x=8
    lcd.print(".");
    delay(300);
    lcd.setCursor(9, 1);      //line=1, x=9
    lcd.print(".");
    delay(300);
    lcd.setCursor(10, 1);      //line=1, x=10
    lcd.print(".");
    delay(300);
    lcd.clear();

    
}

// Convert ADC value to key number
uint16_t get_key(unsigned int input)
{
    uint16_t k;
    for (k = 0; k < NUM_KEYS; k++)
        if (input < adc_key_val[k])
            return k;

    if (k >= NUM_KEYS)
        k = -1;     // No valid key pressed

    return k;
}

void ADC_init(void) {
    // Referencia AVCC, canal ADC0
    // El ADC medirá el voltaje presente en el pin A0, usando AVCC (5v) como referencia
    // ADMUX configura referencia y canal del ADC
    ADMUX = (1 << REFS0);

    // En ADCSRA configuramos el prescaler para que trabaje de forma segura entre 50 y 200 kHz
    // El reloj del Arduino UNO es de 16MHz, demasiado rápido para el ADC, así que hay que dividirlo
    //
    // Prescaler = 128 (16MHz/128 = 125kHz)
    // 2,      4,        8,        16,       32,       64,       128
    // 001,    010,      011,      100,      101,      110,      111
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    // Habilitar el módulo ADC (ADEN) y habilita la interrupción que se dispara al terminar cada conversión (genera ADC_vect) (ADIE)
    ADCSRA |= (1 << ADEN) | (1 << ADIE);

    // Free running
    // El ADC lanzará conversiones continuamente sin que haya que poner ADSC cada vez
    ADCSRA |= (1 << ADATE);

    // Arrancar conversión
    // ADSC inicia la primera conversión
    // En Free Running, al terminar una conversión ADSC se vuelve a iniciar automáticamente.
    ADCSRA |= (1 << ADSC);
}

ISR(ADC_vect) {
    // Leer valor ADC
    // Lectura ordenada: primero ADCL y luego ADCH
    // “bloquear” correctamente el par de bytes y no perder la coherencia del resultado
    uint8_t low  = ADCL;
    uint8_t high = ADCH;

    // Combinar ambos bytes high y low en un solo valor de 10 bits
    // El resultado del ADC es un valor entre 0 y 1023 (10 bits)
    adc_value = (high << 8) | low;

    tecla = get_key(adc_value);

    // Encolar la función para ser ejecutada en el loop principal

    // Si hay cambio de estado
    if (tecla != oldkey) {
        if (tecla != -1 && key_down_handler) {
            int tecla_copy = tecla;
            fnqueue_add([=](){ key_down_handler(tecla_copy); });
        }
        if (oldkey != -1 && key_up_handler) {
           int oldkey_copy = oldkey;
           fnqueue_add([=](){ key_up_handler(oldkey_copy); });
        }
        oldkey = tecla;
    }
}

void onKeyDown(int tecla) {
    lcd.setCursor(0, 1);
    lcd.print("Presionada: ");
    lcd.print(tecla);
}

void onKeyUp(int tecla) {
    lcd.setCursor(0, 1);
    lcd.print("Liberada: ");
    lcd.print(tecla);
}

