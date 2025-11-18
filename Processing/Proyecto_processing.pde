import processing.serial.*;
import processing.sound.*;
import java.io.PrintWriter; //Para archivo .csv

Serial myPort;
SoundFile alarma; //Objeto para archivo de sonido
PrintWriter outputCSV; //Objeto para el .csv

class Graficador {
  float xPos = 0;
  float prevY;
  boolean primerPunto = true;

  //Constructor
  Graficador() {
    prevY = height - 10;
    dibujarGrilla();
  }
  //método
  void dibujarGrilla() {
    noStroke();
    fill(255);
    rect(0, 40, width, height - 40); 
    //dibujo grilla
    stroke(200); // Gris claro
    strokeWeight(1);

    for (int i = 0; i < width; i += 50) {
      line(i, 40, i, height);
    }
    
    for (int i = 40; i < height; i += 50) {
      line(0, i, width, i);
    }
  }
  //metodo
  void dibujar(int valor) {
    float yPos = map(valor, 0, 1100, height - 10, 40);
    
    if (primerPunto) {
      prevY = yPos;
      primerPunto = false;
    }
    //reinicio pantalla
    if (xPos >= width) {
      xPos = 0;
      prevY = yPos;
      dibujarGrilla(); //Redibujo grilla limpia
    }

    //configuracion estilo línea
    stroke(200, 0, 0); //rojo oscuro
    strokeWeight(1.5); //ancho
    
    line(xPos - 2, prevY, xPos, yPos); 
    prevY = yPos;
    xPos += 2;      //zoom horizontal
  }
}

int valorRecibido = 0;      // Variable temporal para guardar dato
boolean hayNuevoDato = false; // Bandera para el draw
int UMBRAL_PC = 100; //Umbral para la alerta en computadora
boolean enAlertaPC = false;
int maxAmplitudEvento = 0;
long tiempoInicioEvento = 0;
long tiempoUltimaVibracion = 0;
int TIEMPO_ESPERA = 1000;

Graficador miGrafico;  //Objeto de clase Graficador

void setup() {
  size(1000, 500);
  background(255);
  
  miGrafico = new Graficador();

  //Inicializo puerto serie
  try {
    String portName = "Com5";
    myPort = new Serial(this, portName, 115200);
    myPort.bufferUntil('\n');
    println("Puerto " + portName + " abierto.");
  } catch (Exception e) {
      println("Error al abrir el puerto");
      e.printStackTrace();
    }
    
//Preparar archivo de log (.csv)
  try {
    String nombreArchivo = "sismografo_" + hour() + "-" + minute() + "-" + second() + ".csv";
    
    outputCSV = new PrintWriter(new java.io.FileWriter(nombreArchivo, true));
    outputCSV.println("FechaHora;ValorSensor");
    outputCSV.flush(); 
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

  // Sincroniza placa con el umbral de la PC al empezar
  if (myPort != null) {
    myPort.write("s" + UMBRAL_PC + "\n");
  }
}

//Bucle draw
void draw() {
  if (enAlertaPC) {
    fill(255, 0, 0); 
  } else {
    fill(173, 216, 230); 
  }
  
  noStroke();
  rect(0, 0, width, 40); 
  
  fill(0); 
  textSize(16);
  text("Sismógrafo en Tiempo Real | UMBRAL PC: " + UMBRAL_PC + " (Presiona 'U' para subir / 'J' para bajar)", 10, 25);
  if (hayNuevoDato) {
    miGrafico.dibujar(valorRecibido); //Dibujamos en hilo principal
    hayNuevoDato = false;
  }
}

void serialEvent(Serial p) {
  String data = p.readStringUntil('\n');
  if (data == null) return;
  
  data = trim(data); 

  try {
    int sensorValue = int(data);

    String fechaLegible = day() + "/" + month() + "/" + year() + " " + hour() + ":" + minute() + ":" + second();
    outputCSV.println(fechaLegible + ";" + sensorValue);
    outputCSV.flush();

    //guardo dato para draw
    valorRecibido = sensorValue;
    hayNuevoDato = true;
    
    if (sensorValue > UMBRAL_PC) {      
      
      //actualizo reloj
      tiempoUltimaVibracion = millis(); 
      
      if (!enAlertaPC) {
        enAlertaPC = true;             
        tiempoInicioEvento = millis(); 
        maxAmplitudEvento = sensorValue; 
        
        if (alarma != null && !alarma.isPlaying()) {
          alarma.play();
        }
      }
      
      if (sensorValue > maxAmplitudEvento) {
        maxAmplitudEvento = sensorValue;
      }
      
    } else {
        
        if (enAlertaPC && (millis() - tiempoUltimaVibracion > TIEMPO_ESPERA)) {        
          enAlertaPC = false;
          
          if (alarma != null) {
            alarma.stop();
          }
          
          //calculo duracion evento
          long duracionEvento = millis() - tiempoInicioEvento - TIEMPO_ESPERA;
          
          println("--- FIN DE EVENTO ---");
          println("  Duración: " + (duracionEvento / 1000.0) + " segundos");
          println("  Amplitud Máxima: " + maxAmplitudEvento);
          println("-----------------------");
        }
    }
  } catch (Exception e) {}    
}

//control umbral de sensibilidad
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
    // "s" es el prefijo que entiende el arduino
    myPort.write("s" + UMBRAL_PC + "\n");
    println("Enviando nuevo umbral al Arduino: " + UMBRAL_PC);
  }
}
