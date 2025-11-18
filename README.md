 Informatica-Brizuela-Ferrer

Proyecto: Monitor de Vibración con Arduino y Processing

Descripción:

Este proyecto es un sistema de monitoreo de vibraciones en tiempo real.

Un Arduino se encarga de:
1.  Leer constantemente un sensor de vibración analógico.
2.  Activar alertas locales (LED, Buzzer y Pantalla OLED) si la vibración supera un umbral.
3.  Enviar todos los datos del sensor a la PC por el puerto serie.

En Processing se encarga de:
1.  Recibir los datos del Arduino.
2.  Graficar la señal de vibración en tiempo real.
3.  Registrar los datos en un archivo .csv
4.  Reproducir un sonido de alarma.

Ambos sistemas se comunican bidireccionalmente: la PC también puede enviar un nuevo umbral de alerta al Arduino sin necesidad de reprogramarlo.

Cómo Funciona el Flujo de Datos

1.  Arduino -> PC:El Arduino envía constantemente el valor crudo del sensor a través de Serial.println() y processing recibe este número, lo usa para dibujar la gráfica y lo guarda en el log.
2.  PC -> Arduino:Processing puede enviar un string al Arduino.El Arduino lo detecta, extrae el número y actualiza el 'UMBRAL_ALERTA'.