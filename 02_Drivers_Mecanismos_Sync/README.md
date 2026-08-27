# Laboratorio 2: Drivers Modulares, Teclado/LCD Shield y Colas de Funciones (Event-Driven)

## 📌 Descripción General
Este laboratorio aborda el diseño de arquitectura de software para sistemas embebidos mediante **drivers modulares**, **secciones críticas**, **colas de ejecución diferida (Function Queues)** y **máquinas de estados finitos (FSM)**. Se implementa un sistema interactivo completo con pantalla LCD 16x2 y teclado analógico (LCD Keypad Shield), capaz de ejecutar un cronómetro de precisión con eventos desacoplados de las interrupciones.

---

## 🎯 Objetivos de Aprendizaje
- **Modularidad:** Separación estricta entre capa de abstracción de hardware (HAL), drivers (`.h` / `.cpp`) y lógica de aplicación (`main.cpp`).
- **Mecanismos de Sincronización:** Identificación de condiciones de carrera y protección de recursos compartidos mediante **secciones críticas** (`critical.h`).
- **Patrón Event-Driven (Function Queue):** Implementación de una cola circular de punteros a función (`fnqueue.h`) para diferir el procesamiento pesado fuera de las Rutinas de Servicio de Interrupción (ISR).
- **Driver de Teclado y Display:** Mapeo de tensiones del conversor analógico del Shield a teclas (`SELECT`, `LEFT`, `UP`, `DOWN`, `RIGHT`) con máquina de estados para filtrado y eventos de pulsación corta/larga.
- **Aplicación Cronómetro:** Lógica de temporización precisa con comandos interactivos por display.

---

## 📂 Estructura del Directorio
```text
02_Drivers_Mecanismos_Sync/
├── src/
│   ├── Actividad3-main.cpp         # Punto de entrada principal y despachador de eventos
│   ├── Actividad3-Driver.cpp       # Implementación del driver de teclado y periféricos
│   ├── critical.cpp                # Manejo seguro de secciones críticas (habilitación/deshabilitación atómica de IRQs)
│   └── fnqueue.cpp                 # Implementación de la cola de funciones diferidas
├── include/
│   ├── Actividad3-Driver.h         # API pública del driver de teclado/display
│   ├── critical.h                  # Macros y funciones de sección crítica
│   └── fnqueue.h                   # Interfaz de la cola de funciones
├── deploy/
│   ├── Actividad1-LCDShieldNewTest.cpp
│   ├── Actividad2-Driver.cpp / .h
│   ├── Actividad2-LCDShieldInterrupted.cpp
│   ├── Cronometro.cpp / .h         # Lógica del cronómetro de precisión
│   ├── LCD1602KeyShieldTest.cpp
│   └── servo.cpp                   # Driver para servomotores
├── ejemplos/                       # Ejemplos de Programación Orientada a Objetos en C++ embebido
├── docs/                           # Presentaciones y consignas del laboratorio
└── platformio.ini                  # Configuración PlatformIO
```

---

## 🏗️ Arquitectura del Sistema (Event-Driven)
```text
 ┌──────────────────────┐
 │ Interrupción Timer   │ ──► [ Encola callback ] ──┐
 └──────────────────────┘                           ▼
 ┌──────────────────────┐                     ┌───────────────┐
 │ Interrupción Botón   │ ──► [ Encola callback ] ─►│ FunctionQueue │
 └──────────────────────┘                           └───────┬───────┘
                                                            │ Desencola en main loop
                                                            ▼
                                                   ┌─────────────────┐
                                                   │ Ejecuta función │
                                                   └─────────────────┘
```

---

## 🛠️ Hardware Utilizado
- Arduino Uno (ATmega328P)
- LCD Keypad Shield 1602 (Hitachi HD44780 + escalera resistiva de pulsadores en A0)
- Servomotor estándar SG90
