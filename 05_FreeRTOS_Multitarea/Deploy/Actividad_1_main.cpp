#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

// Pin del LED incorporado
#define LED_PIN LED_BUILTIN

// Semáforos para sincronizar las tareas
static SemaphoreHandle_t xLedSemaphore1 = NULL;
static SemaphoreHandle_t xLedSemaphore2 = NULL;

// Tarea para encender el LED
void vTaskTurnOn(void *pvParameters)
{
    (void) pvParameters;
    for (;;)
    {
        if (xSemaphoreTake(xLedSemaphore1, portMAX_DELAY) == pdTRUE)
        {
            digitalWrite(LED_PIN, HIGH);
            vTaskDelay(pdMS_TO_TICKS(500)); 
            xSemaphoreGive(xLedSemaphore2);
        }
    }
}

// Tarea para apagar el LED
void vTaskTurnOff(void *pvParameters)
{
    (void) pvParameters;
    for (;;)
    {
        if (xSemaphoreTake(xLedSemaphore2, portMAX_DELAY) == pdTRUE)
        {
            digitalWrite(LED_PIN, LOW);
            vTaskDelay(pdMS_TO_TICKS(500)); 
            xSemaphoreGive(xLedSemaphore1);
        }
    }
}

void setup()
{
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    xLedSemaphore1 = xSemaphoreCreateBinary();
    xLedSemaphore2 = xSemaphoreCreateBinary();
    if (xLedSemaphore1 == NULL || xLedSemaphore2 == NULL)
    {
        // Fallo en creacion de semaforo
        for (;;);
    }

    // Se fuerza el primer take para iniciar la secuencia
    xSemaphoreGive(xLedSemaphore1);

    xTaskCreate(vTaskTurnOn, "LED On", 128, NULL, 2, NULL);
    xTaskCreate(vTaskTurnOff, "LED Off", 128, NULL, 1, NULL);
}

void loop()
{
    // Empty - FreeRTOS scheduler is running
}
