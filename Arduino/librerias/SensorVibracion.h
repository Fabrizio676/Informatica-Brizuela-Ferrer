#ifndef SENSORVIBRACION_H
#define SENSORVIBRACION_H

#include <Arduino.h>

class SensorVibracion {
  private:
    int pin;

  public:
    // Constructor
    SensorVibracion(int pin);

    // Métodos
    void inicializar();
    int leerValorAnalogico(); // Devuelve un valor entre 0 y 1023
};

#endif