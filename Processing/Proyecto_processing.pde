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
