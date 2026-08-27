# Referencias y Recursos

## Documentación Técnica

### Especificaciones I2C

- **ATmega328P Datasheet**: Documentación del microcontrolador Arduino Uno
  - URL: https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
  - Secciones relevantes: Two-wire Serial Interface (TWI), ADC

- **Raspberry Pi I2C Documentation**: Configuración y uso de I2C en Raspberry Pi
  - URL: https://www.raspberrypi.org/documentation/hardware/raspberrypi/
  - Herramientas: i2c-tools, SMBus library

### Protocolos de Comunicación

- **RFC 1055 - Serial Line Internet Protocol (SLIP)**: Inspiración para escape de caracteres
  - URL: https://tools.ietf.org/html/rfc1055
  - Relevancia: Técnicas de framing y escape

- **HDLC Protocol**: High-Level Data Link Control
  - Estándar: ISO/IEC 13239
  - Aplicación: Técnicas de delimitación de tramas

## Librerías y Herramientas

### Arduino

```cpp
// Librerías utilizadas
#include <Wire.h>        // Comunicación I2C
#include <Arduino.h>     // Framework Arduino
```

**Wire Library Documentation:**
- Métodos: `Wire.begin()`, `Wire.onRequest()`, `Wire.onReceive()`
- Limitaciones: Buffer de 32 bytes en Arduino Uno
- URL: https://www.arduino.cc/en/reference/wire

### Raspberry Pi

#### Implementación en C con pigpio

```c
// Librerías utilizadas
#include <pigpio.h>     // Comunicación I2C de alta performance
#include <stdio.h>      // I/O estándar
#include <stdlib.h>     // Memoria dinámica y utilidades
#include <string.h>     // Manipulación de strings
#include <unistd.h>     // System calls POSIX
#include <stdint.h>     // Tipos de datos de tamaño fijo
#include <time.h>       // Funciones de tiempo
```

**pigpio C Library:**
- Headers: `#include <pigpio.h>`
- Compilación: `gcc -o programa programa.c -lpigpio -lrt -lpthread`
- Funciones clave: `gpioInitialise()`, `i2cOpen()`, `i2cWriteDevice()`, `i2cReadDevice()`
- Documentación: [http://abyz.me.uk/rpi/pigpio/cif.html](http://abyz.me.uk/rpi/pigpio/cif.html)

#### Implementación en Python con pigpio

```python
# Librerías utilizadas
import pigpio          # Comunicación I2C precisa y robusta
import struct          # Conversión de datos binarios
import time            # Delays y timing
import argparse        # Parsing de argumentos CLI
```

**pigpio Python Library:**
- Instalación: `sudo apt install pigpio python3-pigpio`
- Ventajas: Mayor precisión de timing, mejor manejo de errores, soporte completo GPIO
- Documentación: [http://abyz.me.uk/rpi/pigpio/](http://abyz.me.uk/rpi/pigpio/)



### Herramientas de Desarrollo

**i2c-tools (Raspberry Pi):**
```bash
sudo apt install i2c-tools

# Comandos útiles:
i2cdetect -y 1          # Detectar dispositivos
i2cget -y 1 0x08        # Leer byte de dispositivo
i2cset -y 1 0x08 0x01   # Escribir byte a dispositivo
i2cdump -y 1 0x08       # Volcar contenido de dispositivo
```

**PlatformIO:**
- URL: https://platformio.org/
- Configuración para Arduino Uno:
```ini
[env:uno]
platform = atmelavr
board = uno
framework = arduino
```


### Conexiones Hardware

```
Raspberry Pi GPIO        Arduino Uno
─────────────────        ───────────
Pin 3 (GPIO 2) SDA   →   A4 (SDA)
Pin 5 (GPIO 3) SCL   →   A5 (SCL)
Pin 2 (5V)           →   5V or VIN
Pin 6 (GND)          →   GND

Resistencias Pull-up (opcionales):
4.7kΩ entre SDA y 5V
4.7kΩ entre SCL y 5V
```

### Problema: "Remote I/O error"

**Causas posibles:**
- I2C no habilitado en Raspberry Pi
- Conexiones incorrectas
- Dispositivo no responde

**Soluciones:**
```bash
# Verificar I2C habilitado
sudo raspi-config

# Verificar conexiones físicas
sudo i2cdetect -y 1

```

#### Problema: Buffer overflow en Arduino

**Causa:** Wire library limitada a 32 bytes

**Solución:**
```cpp
// Dividir mensajes grandes
void send_large_data(uint8_t* data, uint16_t size) {
    const uint8_t chunk_size = 30;  // Dejar margen
    
    for (uint16_t i = 0; i < size; i += chunk_size) {
        uint8_t remaining = min(chunk_size, size - i);
        Wire.write(&data[i], remaining);
        
        if (remaining == chunk_size) {
            delay(10);  // Pausa entre chunks
        }
    }
}
```

