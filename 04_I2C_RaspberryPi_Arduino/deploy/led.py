"""
Este script controla un LED conectado al pin GPIO 18 de una Raspberry Pi.

Funcionamiento:
- Configura el pin 18 como salida.
- Enciende el LED durante 1 segundo y luego lo apaga.

Requisitos:
- La librería RPi.GPIO debe estar instalada.
# La librería para controlar los pines GPIO en Raspberry Pi es: RPi.GPIO
# Para instalarla, ejecuta en la terminal:
# sudo apt-get update
# sudo apt-get install python3-rpi.gpio
# O usando pip:
# pip install RPi.GPIO
- El LED debe estar correctamente conectado al pin GPIO 18.

Uso:
Ejecutar el script para encender y apagar el LED automáticamente.
"""

#!/usr/bin/env python3
import RPi.GPIO as GPIO
import time
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(18,GPIO.OUT)
print("LED on")
GPIO.output(18,GPIO.HIGH)
time.sleep(1)
print("LED off")
GPIO.output(18,GPIO.LOW)

"""
Ejecutar el script

sudo python3 led.py
*** 
El sudo es importante porque el acceso a los pines GPIO requiere privilegios de administrador en Raspberry Pi.
"""