#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <timers.h>

// Pines de los LEDs
#define LED1_PIN 12  
#define LED2_PIN 11  

// Timer handles
TimerHandle_t xTimer1 = NULL;
TimerHandle_t xTimer2 = NULL;

// Periodos de los temporizadores 
const uint32_t TIMER1_PERIOD = 125;  // 4Hz = 250ms 
uint32_t timer2Period = 500;         // 1Hz = 1000ms 

// Timer callback para LED1 (4Hz)
void vTimer1Callback(TimerHandle_t xTimer) {
    digitalWrite(LED1_PIN, !digitalRead(LED1_PIN));
}

// Timer callback para LED2 (frecuencia variable)
void vTimer2Callback(TimerHandle_t xTimer) {
    digitalWrite(LED2_PIN, !digitalRead(LED2_PIN));
}

void setup() {

    Serial.begin(9600);
    
    // Configuro los pines de los LEDs como salidas e incialmente apagados
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);

    // Timer para LED1 (4Hz)
    xTimer1 = xTimerCreate(
        "Timer1",                       // Nombre
        pdMS_TO_TICKS(TIMER1_PERIOD),   // El periodo
        pdTRUE,                         // Auto-reload
        (void*)0,                       // Timer ID
        vTimer1Callback                 // Callback función
    );

    // timer para LED2 (frecuencia variable)
    xTimer2 = xTimerCreate(
        "Timer2",               
        pdMS_TO_TICKS(timer2Period), 
        pdTRUE,                
        (void*)1,               
        vTimer2Callback         
    );

    xTimerStart(xTimer1, 0);
    xTimerStart(xTimer2, 0);
    
}

void loop() {
   
    if (Serial.available() > 0) {
        char input = Serial.read();
        
        switch (input) {
            case '1': // 0.5Hz
                timer2Period = 1000; 
                xTimerChangePeriod(xTimer2, pdMS_TO_TICKS(timer2Period), 0);
                Serial.println("LED2 cambio de frecuencia a 0.5Hz");
                break;
                
            case '2': // 2Hz
                timer2Period = 250;   
                xTimerChangePeriod(xTimer2, pdMS_TO_TICKS(timer2Period), 0);
                Serial.println("LED2 cambio de frecuencia a 2Hz");
                break;
        }
    }
    

}