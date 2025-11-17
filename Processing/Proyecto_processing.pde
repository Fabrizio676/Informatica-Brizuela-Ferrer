import processing.serial.*;
import processing.sound.*;
import java.io.PrintWriter; //Para archivo .csv

Serial myPort;
SoundFile alarma; //Objeto para el archivo de sonido
PrintWriter outputCSV; //Objeto para el .csv

class Graficador {
  float xPos = 0;
  float prevY;

  //Constructor
  Graficador() {
    prevY = height / 2; //Inicia en el centro
  }

  //Método
  void dibujar(int valor) {
    float yPos = map(valor, 0, 1023, height - 10, 10);
    stroke(0);
    line(xPos, prevY, xPos + 1, yPos);
    prevY = yPos;
    xPos++;

    if (xPos > width) {
      xPos = 0;
      background(255);    //Reiniciar gráfico si llega al final
    }
  }
}

int UMBRAL_PC = 100; //Umbral para la alerta en computadora
boolean enAlertaPC = false;
int maxAmplitudEvento = 0;
long tiempoInicioEvento = 0;

Graficador miGrafico;  //Objeto de clase Graficador

void setup() {
  size(1000, 500);
  background(255);
  
  miGrafico = new Graficador();

  //Inicializo puerto serie
  try {
    String portName = Serial.list()[0];
    myPort = new Serial(this, portName, 115200);
    myPort.bufferUntil('\n');
    println("Puerto " + portName + " abierto.");
  } catch (Exception e) {
      println("Error al abrir el puerto");
      e.printStackTrace();
    }
    
  //Preparar archivo de log (.csv)
  try {
    outputCSV = new PrintWriter(new java.io.FileWriter("sismografo_log.csv", true));
    outputCSV.println("Timestamp,ValorSensor");
    outputCSV.flush(); //guardar cambios
  } catch (java.io.IOException e) {
      e.printStackTrace();
    }

  String nombreArchivoAlarma = "alarma.mp3";
  try {
    alarma = new SoundFile(this, nombreArchivoAlarma);
    println("Archivo de sonido '" + nombreArchivoAlarma + "' cargado.");
  } catch (Exception e) {
      println("Error al cargar el sonido. ¿El archivo '" + nombreArchivoAlarma + "' está en la carpeta 'data'?");
    }

  // Sincroniza el Arduino con el umbral de la PC al empezar
  if (myPort != null) {
    myPort.write("s" + UMBRAL_PC + "\n");
  }
}
