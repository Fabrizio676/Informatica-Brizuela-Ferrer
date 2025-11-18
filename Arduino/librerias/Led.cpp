#include "Led.h"

// Constructor
Led::Led(byte pin) {
  this->pin = pin;
}

// Métodos
void Led::inicializar() {
  pinMode(pin, OUTPUT);
  apagar();
}

void Led::encender() {
  digitalWrite(pin, HIGH);
}

void Led::apagar() {
  digitalWrite(pin, LOW);
}