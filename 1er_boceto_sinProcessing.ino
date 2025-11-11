//Librerías
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>     // Librería para el display OLED
//Definiciones
#define PIN_SENSOR     D5
#define PIN_LED   D7
#define PIN_BUZZER     D6

//Configuración del Display OLED (I2C)
#define ANCHO_PANTALLA   128  // Ancho del OLED
#define ALTO_PANTALLA    64   // Alto del OLED
#define OLED_RESET     -1   // Sin pin de reset

//Clase LED
class Led {
private:
    byte pin; //Atributo privado

public:
    //Constructor
    Led(byte pin) {
        this->pin = pin;
    }

    //Método de inicialización
    void inicializar() {
        pinMode(pin, OUTPUT);
        apagar(); //Estado inicial apagado
    }

    //Método para encender
    void encender() {
        digitalWrite(pin, HIGH);
    }

    //Método para apagar
    void apagar() {
        digitalWrite(pin, LOW);
    }
};

//Clase del sensor de vibraciones
class SensorVibracion {
private:
    int pin; // Atributo privado

public:
    //Constructor
    SensorVibracion(int pin) {
        this->pin = pin;
    }

    //Método de inicialización
    void inicializar() {
        pinMode(pin, INPUT); // El sensor es una entrada
    }

    //Método público para leer el estado del sensor
    bool hayVibracion() {
        //La salida digital del SW420 es LOW (Baja) cuando detecta vibración
        return (digitalRead(pin) == LOW);
    }
};

//Clase del Buzzer
class Buzzer {
private:
    int pin;

public:
    //Constructor
    Buzzer(int pin) {
        this->pin = pin;
    }

    //Método de inicialización
    void inicializar() {
        pinMode(pin, OUTPUT);
    }

    //Método para activar la alerta
    void sonarAlerta() {
        //Genera un tono de 1500Hz por 500ms
        tone(pin, 1500, 500);
    }

    //Método para silenciar
    void silenciar() {
        noTone(pin);
    }
};

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
