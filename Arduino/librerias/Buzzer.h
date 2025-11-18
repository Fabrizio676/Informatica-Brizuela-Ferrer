#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

class Buzzer {
  private:
    int pin;

  public:
    // Constructor
    Buzzer(int pin);

    // Métodos
    void inicializar();
    void sonarAlerta();
    void silenciar();
};

#endif