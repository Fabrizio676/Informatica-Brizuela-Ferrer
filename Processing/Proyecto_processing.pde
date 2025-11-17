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

//Bucle draw
void draw() {
  if (enAlertaPC) {
    fill(255, 0, 0, 100); //pantalla roja con transparencia
    rect(0, 0, width, height);
  }
  
  //interfaz de usuario (Texto)
  fill(0); // Texto negro
  rect(0, 0, width, 40); // Barra superior blanca para el texto
  fill(0);
  textSize(16);
  text("Sismógrafo en Tiempo Real | UMBRAL PC: " + UMBRAL_PC + " (Presiona 'U' para subir / 'J' para bajar)", 10, 25);
}

void serialEvent(Serial p) {
  String data = p.readStringUntil('\n');
  if (data == null) return;
  
  data = trim(data); //limpiar espacios

  try {
    //1ero: Leo y convierto el dato
    int sensorValue = int(data);
    
    //2do: Registro el dato en archivo .csv
    long timestamp = System.currentTimeMillis();
    outputCSV.println(timestamp + "," + sensorValue);
    outputCSV.flush();

    //3ro: Dibujar Gráfico
    miGrafico.dibujar(sensorValue);
    
    if (sensorValue > UMBRAL_PC) {     //Si valor medido supera umbral se activa alerta
      if (!enAlertaPC) {
        enAlertaPC = true;             //Si no estaba en alerta, ahora lo estoy
        tiempoInicioEvento = millis(); //Registro tiempo de inicio
        maxAmplitudEvento = sensorValue; //Registro primera amplitud de vibracion
        
        //Sonar alarma (si no está sonando)
        if (alarma != null && !alarma.isPlaying()) {
          alarma.play();
        }
      }
      
      //Actualizar amplitud máxima durante el evento
      if (sensorValue > maxAmplitudEvento) {
        maxAmplitudEvento = sensorValue;
      }
    } else {
        //fin de alerta
        if (enAlertaPC) {        //si estoy en alerta, salgo de ese modo
          enAlertaPC = false;
          long duracionEvento = millis() - tiempoInicioEvento;
          
          //Imprimir análisis en consola
          println("--- FIN DE EVENTO ---");
          println("  Duración: " + (duracionEvento / 1000.0) + " segundos");
          println("  Amplitud Máxima: " + maxAmplitudEvento);
          println("-----------------------");
        }
    }
  } catch (Exception e) {}    // Ignorar si el dato no es un número
}

//funcion para controlar umbral de sensibilidad
void keyPressed() {
  if (key == 'u' || key == 'U') {
    UMBRAL_PC += 10; // Aumentar umbral
  } else if (key == 'j' || key == 'J') {
    if (UMBRAL_PC > 10) { // Evitar que sea negativo
      UMBRAL_PC -= 10; // Disminuir umbral
    }
  }

  // Enviar el nuevo umbral al Arduino
  if (myPort != null) {
    // "s" es el prefijo que el Arduino entenderá
    myPort.write("s" + UMBRAL_PC + "\n");
    println("Enviando nuevo umbral al Arduino: " + UMBRAL_PC);
  }
}
