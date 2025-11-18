//Librerías externas
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Librerias Propias
#include "Led.h"
#include "Buzzer.h"
#include "SensorVibracion.h"

//Definiciones de pines
#define PIN_SENSOR     A0
#define PIN_LED        D7
#define PIN_BUZZER     D6

//Configuración del Display OLED
#define ANCHO_PANTALLA   128   
#define ALTO_PANTALLA    64 
#define OLED_RESET     -1

//Instancias
Adafruit_SSD1306 display(ANCHO_PANTALLA, ALTO_PANTALLA, &Wire, OLED_RESET);
Led ledAlerta(PIN_LED);
SensorVibracion sensor(PIN_SENSOR);
Buzzer buzzer(PIN_BUZZER);

//variables globales
bool enAlerta = false;
int UMBRAL_ALERTA = 100;

//Variables para cooldown)
unsigned long tiempoUltimaVibracion = 0; 
const int TIEMPO_ESPERA = 1000; // Esperar 2 segundos antes de apagar la alerta

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
    
    actualizarDisplay("Vigilando", "Umbral: " + String(UMBRAL_ALERTA), 2);
}

void loop() {
    revisarSerialEntrante();
    int valorSensor = sensor.leerValorAnalogico();
    //envio dato a processing
    Serial.println(valorSensor);

    bool vibracionDetectada = (valorSensor > UMBRAL_ALERTA);
    
    if (vibracionDetectada) {
        // Actualizamos el reloj de la ultima medicion
        tiempoUltimaVibracion = millis();

        buzzer.sonarAlerta(); 
        
        //si no estaba en alerta, la activo
        if (!enAlerta) { 
            enAlerta = true; 
            ledAlerta.encender();
            actualizarDisplay("¡ALERTA!", "Vibracion!", 2);
        }
    }
    else {
        //si no hay vibracion espero un segundo y desactivo modo alerta
        if (enAlerta && (millis() - tiempoUltimaVibracion > TIEMPO_ESPERA)) {
            enAlerta = false;
            ledAlerta.apagar();
            buzzer.silenciar();
            
            actualizarDisplay("Vigilando", "Umbral: " + String(UMBRAL_ALERTA), 2);
        }
    }
    delay(20);
}

void revisarSerialEntrante() {    
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();
    
    if (comando.startsWith("s")) {
      String valorStr = comando.substring(1);
      int nuevoUmbral = valorStr.toInt();
      
      if (nuevoUmbral > 0) {
        UMBRAL_ALERTA = nuevoUmbral;
        
        //actualizo pantalla si cambiamos umbral
        if (!enAlerta) {
          actualizarDisplay("Vigilando", "Umbral:" + String(UMBRAL_ALERTA), 2);
        }
      }
    }
  }
}

//Función para actualizar display
void actualizarDisplay(String linea1, String linea2, int tamanoLinea1) {
    display.clearDisplay();
    display.setCursor(0, 0);
    
    //Línea 1 (Título)
    display.setTextSize(tamanoLinea1); 
    display.setTextColor(SSD1306_WHITE);
    display.println(linea1);

    //Línea 2 (Subtítulo)
    display.println("");
    display.setTextSize(2);
    display.println(linea2);
    
    display.display();
}