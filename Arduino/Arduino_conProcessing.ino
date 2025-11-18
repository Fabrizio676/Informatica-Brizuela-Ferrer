//Librerías
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Definiciones
#define PIN_SENSOR     A0
#define PIN_LED        D7
#define PIN_BUZZER     D6

int UMBRAL_ALERTA = 100;

//Configuración del Display OLED
#define ANCHO_PANTALLA   128   
#define ALTO_PANTALLA    64 
#define OLED_RESET     -1

//Clase LED
class Led {
private:
    byte pin;
public:
    Led(byte pin) { this->pin = pin; }
    void inicializar() { pinMode(pin, OUTPUT); apagar(); }
    void encender() { digitalWrite(pin, HIGH); }
    void apagar() { digitalWrite(pin, LOW); }
};

//Clase del sensor de vibraciones
class SensorVibracion {
private:
    int pin;
public:
    SensorVibracion(int pin) { this->pin = pin; }
    void inicializar() { pinMode(pin, INPUT); }
    int leerValorAnalogico() {
        return analogRead(pin); //Leo valor del sensor
    }
};

//Clase del Buzzer
class Buzzer {
private:
    int pin;
public:
    Buzzer(int pin) { this->pin = pin; }
    void inicializar() { pinMode(pin, OUTPUT); }
    void sonarAlerta() { tone(pin, 1500, 500); }
    void silenciar() { noTone(pin); }
};

//Instancias
Adafruit_SSD1306 display(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, OLED_RESET);
Led ledAlerta(PIN_LED);
SensorVibracion sensor(PIN_SENSOR);
Buzzer buzzer(PIN_BUZZER);

bool enAlerta = false;

void setup() {
   Serial.begin(115200);
   Wire.begin();

    //Inicializo objetos
    ledAlerta.inicializar();
    sensor.inicializar();
    buzzer.inicializar();

    //Inicializo display
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("No se pudo iniciar el display"));
        while (true);
    }
    actualizarDisplay("Monitoreando...", "Umbral: " + String(UMBRAL_ALERTA), 1);
}

void loop() {
    //1ero: REVISO SI LLEGO UN NUEVO UMBRAL DESDE PROCESSING
    revisarSerialEntrante();

    //2do: LEO VALOR DEL SENSOR
    int valorSensor = sensor.leerValorAnalogico();

    //3ro: ENVIAMOS EL DATO A PROCESSING (para gráfico y log)
    Serial.println(valorSensor);

    bool vibracionDetectada = (valorSensor > UMBRAL_ALERTA);

    //Caso 1: Se detecta vibración y el estado era reposo
    if (vibracionDetectada && !enAlerta) { 
        enAlerta = true; 
        ledAlerta.encender();
        buzzer.sonarAlerta();
        actualizarDisplay("¡ALERTA!", "Vibracion detectada", 2);
    }
    
    //Caso 2: no se detecta vibración y el estado era en alerta
    else if (!vibracionDetectada && enAlerta) {
        enAlerta = false;
        ledAlerta.apagar();
        actualizarDisplay("Monitoreando...", "Umbral: " + String(UMBRAL_ALERTA), 1);
    }

    delay(20);
}

void revisarSerialEntrante() {    //funcion que revisa si processin envio un nuevo umbral
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();
    
    //comando debe empezar con s
    if (comando.startsWith("s")) {
      //Extrae numero después de la s
      String valorStr = comando.substring(1);
      int nuevoUmbral = valorStr.toInt();
      
      if (nuevoUmbral > 0) {
        UMBRAL_ALERTA = nuevoUmbral;
        
        //actualizar el display con el nuevo umbral si no estamos en alerta
        if (!enAlerta) {
          actualizarDisplay("Monitoreando...", "Umbral: " + String(UMBRAL_ALERTA), 1);
        }
      }
    }
  }
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