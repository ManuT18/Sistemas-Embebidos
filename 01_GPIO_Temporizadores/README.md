# Laboratorio 1: GPIO, Temporización, Antirrebote y Comunicación Serie

## 📌 Descripción General
Este laboratorio introduce las bases del desarrollo de firmware para microcontroladores (ATmega328P / Arduino Uno) utilizando **C/C++** y el entorno de compilación **PlatformIO**. Se profundiza en el control de puertos de entrada/salida digital de propósito general (GPIO), manejo eficiente de temporizadores, técnicas de eliminación de rebote (debouncing), detección de flancos y comunicación serie asincrónica (UART).

---

## 🎯 Objetivos de Aprendizaje
- Configuración de pines GPIO como entradas (con pull-up) y salidas.
- Implementación de retardos no bloqueantes utilizando contadores de tiempo (`millis()`, temporizadores por software) en contraste con retardos bloqueantes (`delay()`).
- Implementación de algoritmos de antirrebote (debouncing) por software para lectura confiable de pulsadores.
- Detección precisa de flancos ascendentes y descendentes.
- Transmisión y recepción de datos a través de la interfaz UART (puerto serie).

---

## 📂 Estructura del Directorio
```text
01_GPIO_Temporizadores/
├── src/
│   └── blink.c                          # Ejemplo base de parpadeo y temporización
├── deploy/
│   ├── button.cpp                       # Control de pulsador
│   ├── ButtonArduino.cpp                # Implementación orientada a objetos de botón
│   ├── debounce.c                       # Algoritmo de filtrado de rebotes (debouncing)
│   ├── flanco.c                         # Detección de flancos en señales de entrada
│   ├── EJERCICIO4-TEMPORIZADOYRETARDO.c # Ejercicio de temporización no bloqueante
│   └── EJERCICIO5-SERIAL.c              # Control y comunicación por puerto serie
├── Lab01.pdf                            # Guía y consigna oficial del laboratorio
└── platformio.ini                       # Configuración del entorno PlatformIO (Atmel AVR / Uno)
```

---

## 🛠️ Hardware y Herramientas
- **Microcontrolador:** Microchip ATmega328P (Arduino Uno)
- **Lenguaje:** C / C++
- **Entorno / Toolchain:** PlatformIO / AVR-GCC / AVRDUDE
- **Periféricos:** Pulsadores, LEDs, Resistencias, Cable USB-UART

---

## 🚀 Compilación y Carga
```bash
# Compilar el proyecto
pio run

# Subir al microcontrolador conectado
pio run --target upload

# Abrir el monitor serie (9600 baudios)
pio device monitor -b 9600
```
