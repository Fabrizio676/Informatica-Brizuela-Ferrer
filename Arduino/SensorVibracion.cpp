#include "SensorVibracion.h"

// Constructor
SensorVibracion::SensorVibracion(int pin) {
  this->pin = pin;
}

void SensorVibracion::inicializar() {
  pinMode(pin, INPUT);
}

// Lee valor del sensor
int SensorVibracion::leerValorAnalogico() {
  return analogRead(pin);
}