#include "Buzzer.h"

// Constructor
Buzzer::Buzzer(int pin) {
  this->pin = pin;
}

void Buzzer::inicializar() {
  pinMode(pin, OUTPUT);
  silenciar(); // Aseguramos que empiece en silencio
}

void Buzzer::sonarAlerta() {
  tone(pin, 1500, 500);
}

void Buzzer::silenciar() {
  noTone(pin);
}