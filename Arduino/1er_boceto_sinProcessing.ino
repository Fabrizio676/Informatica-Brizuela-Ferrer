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

//Instancia para el display
Adafruit_SSD1306 display(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, OLED_RESET);

//Instancia para el LED
Led ledAlerta(PIN_LED);

//Instancia para el Sensor de Vibracion
SensorVibracion sensor(PIN_SENSOR);

//Instancia para el Buzzer
Buzzer buzzer(PIN_BUZZER);

//Variable que nos indica si estamos en modo alerta
bool enAlerta = false;

void setup() {
   Serial.begin(115200); //Inicia el puerto serie para depuración
    Wire.begin();         //Inicia el I2C (para el display)

    //Inicializamos objetos
    ledAlerta.inicializar();
    sensor.inicializar();
    buzzer.inicializar();

    //Inicializamos display
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("No se pudo iniciar el display"));
        while (true); //Bucle infinito si falla
    }

    actualizarDisplay("Monitoreando...", "Nivel: Estable", 1);       //Iniciamos el display con el mensaje del modo en reposo

}

void loop() {
 //Leemos el estado del sensor
    bool vibracionDetectada = sensor.hayVibracion();
    
    //Caso 1: Se detecta vibración y el estado era reposo
    if (vibracionDetectada && !enAlerta) {
        enAlerta = true;    //El estado cambia a alerta
        
        //Se ejecutan las acciones de alerta
        ledAlerta.encender();
        buzzer.sonarAlerta(); // Suena una vez
        
        //Actualizamos la pantalla
        actualizarDisplay("¡ALERTA!", "Vibracion detectada", 2);
    }
    
    //Caso 2: no se detecta vibración y el estado era en alerta
    else if (!vibracionDetectada && enAlerta) {
        enAlerta = false;
        
        //Apagamos las alertas
        ledAlerta.apagar();
        
        //Actualizamos la pantalla
        actualizarDisplay("Monitoreando...", "Nivel: Estable", 1);
    }
    delay(50);
}




//Función para actualizar el display
void actualizarDisplay(String linea1, String linea2, int tamanoLinea1) {
    display.clearDisplay();
    display.setCursor(0, 0);
    
    //Línea 1
    display.setTextSize(tamanoLinea1);
    display.setTextColor(SSD1306_WHITE);
    display.println(linea1);

    //Línea 2
    display.setTextSize(1);
    display.println("");
    display.println(linea2);
    
    display.display();
}
