
//-----------------------------------------Librerias-------------------------------------------//

#include "PeticionHttp.h"                                                                                                                     //Librería para la conexión a la red en modo cliente.
#include "DHT.h"                                                                                                                                 //Librería para el dispositivo DTH que captura la temperatura y humedad del ambiente.
#include "ConexionEthernet.h"                                                                                                                    //Clase para la conexión a internet.
#include <NTPClient.h> //importamos la librería del cliente NTP
#include <WiFiUdp.h> // importamos librería UDP para comunicar con NTP
#include <Wire.h>    // Comunicación del modulo I2C  
#include <LiquidCrystal_I2C.h> // importamos las librerías para trabajar con la pantalla LCD 


//------------------------------------------variables------------------------------------------//


#define DHTTYPE DHT11
#define dht_dpin 0                                                                                                                               //D3
DHT dht(dht_dpin, DHTTYPE);
ConexionEthernet Conexion;
PeticionHttp Peticion;
WiFiUDP ntpUDP;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

/* cuando creamos el cliente NTP podemos especificar el servidor al que nos vamos a conectar en este caso 0.south-america.pool.ntp.org SudAmerica.
   También podemos especificar el offset en segundos para que nos muestre la hora según nuestra zona horaria en este caso estamos -18000 (-5*3600=-18000) segundos, ya que estamos en colombia.
   y por ultimo especificamos el intervalo de actualización en mili segundos en este caso 6000*/
NTPClient timeClient(ntpUDP, "0.south-america.pool.ntp.org", -18000, 6000);
int grifo = 14;                                                                                                                                  //D5
int humT = 0;
int humA = 0;
int tempA = 0;
boolean controlGrifo = false;
String url = "http://192.168.1.13:1020/IrrigationSystem/Datos/save";
unsigned long inicioRiego, finalRiego;

//--------------------------------------Inicializador-----------------------------------------//

void setup() {

  Serial.begin(9600);
  pinMode(grifo, OUTPUT);
  digitalWrite(grifo, LOW);
  Conexion.conexion();
  timeClient.begin();
  dht.begin();
  Serial.println("Humedad y Temperatura\n\n");
  lcd.begin(16, 2);   // Columnas y filas de LCD
  lcd.setCursor(0, 0); // Inicio del cursor
  lcd.print("IrrigationSystem");
  delay(3000);
}

//------------------------------------------bucle---------------------------------------------//

void loop() {
  int humedad = analogRead(A0);
  int humedadAmbiente = dht.readHumidity();
  int temperaturaAmbiente = dht.readTemperature();
  int humedadTerreno = map(humedad, 1023, 0, 0, 100);
  Serial.print("la temperatura es: ");
  Serial.println(temperaturaAmbiente);
  Serial.print("la humedad del ambiente es: ");
  Serial.println(humedadAmbiente);
  Serial.print("la humedad del suelo es: ");
  Serial.println(humedadTerreno);
  leerVariablesAmbiente(url, humedadTerreno, humedadAmbiente, temperaturaAmbiente, grifo, controlGrifo);
  oscilarMensaje();
  delay(15000);
}

//------------------método para leer variables y enviarlas por metodo http---------------------//

void leerVariablesAmbiente(String url, int humTerreno, int humAmbiente, int tempAmbiente, int grifo, boolean controlGrifo) {
  String estadoRiego = "";
  String uuid;
  if ((humT != humTerreno) || (tempA != tempAmbiente)) {
    humT = humTerreno;
    tempA = tempAmbiente;
    if (tempA > 25 && humT < 50 && !controlGrifo) {
      controlGrifo = true;
      controlRiego(controlGrifo, grifo);
      inicioRiego = millis();
      estadoRiego = "Inicio";
      uuid = generarUUID();
      Peticion.peticionPost(url, tempA, humT, humA, estadoRiego, 0, uuid);
    } else if (tempA <= 25 && humT >= 50 && controlGrifo) {
      controlGrifo = false;
      controlRiego(controlGrifo, grifo);
      finalRiego = millis();
      estadoRiego = "Final";
      long tiempoRiego = finalRiego - inicioRiego;
      Peticion.peticionPost(url, tempA, humT, humA, estadoRiego, tiempoRiego, uuid);
    }
  }
  Serial.println(controlGrifo);
    mostrarPantalla(tempA, humT, controlGrifo);
}

//---------------------metodo para el control de abrir y cerrar el grifo-----------------------//

void controlRiego(boolean controlGrifo, int grifo) {
  if (controlGrifo) {
    digitalWrite(grifo, HIGH);                                                                                                                   //abrir grifo
    Serial.println("Regando Terreno del Cultivo de Tomates....");
  } else {
    digitalWrite(grifo, LOW);                                                                                                                 //cerrar grifo
    Serial.println("Riego detenido....");
  }
}

//-----------------------------------------Método para generar hora actual-----------------------------------------//

String hora() {
  timeClient.update(); //sincronizamos con el server NTP
  return String(timeClient.getHours()) + String(timeClient.getMinutes()) + String(timeClient.getSeconds());
}

//-------------------------------------Método para generar id con dia actual---------------------------------------//

String idDia() {
  timeClient.update(); //sincronizamos con el server NTP
  return "ID000" + String(timeClient.getDay());
}

//---------------------------------------Método para generar un uuid único-----------------------------------------//

String generarUUID() {
  return idDia() + "-" + "UI" + String(random(1000, 2000)) + "-" + hora() + "-" + "UI" + String(random(1000, 2000));
}

//******************************Métodos para mostrar la información por pantalla lcd*******************************//

//-----------------------------Método para que la pantalla vaya de izquierda a derecha.----------------------------//
void oscilarMensaje() {
    for (int c = 0; c < 12; c++) {                                                                                                                //hace una secuencia hacia la izquierda y luego hacia la derecha, por 12 pasos
      lcd.scrollDisplayLeft();
      delay(600);
    }
    for (int c = 0; c < 12; c++) {
      lcd.scrollDisplayRight();
      delay(600);
    }                                                                                                                                 //blanqueamos la pantalla
}

//--------------------------------------Método para mostrar mensaje encabezado.---------------------------------------//

void encabezadoLcdEstatico() {
  lcd.setCursor(0, 0);
  lcd.print("Irrigation System");                  // Escribimos un Mensaje en la pantalla LCD.
}

//-------------------------------------Método para mostrar informacion en pantalla.----------------------------------//

void mostrarPantalla(int temp, int hum, boolean riego) {
  limpiarLcd();
  encabezadoLcd(riego);
  leerVariables(temp, hum);
}

//----------------------------Método para leer las variables que se mostraran en pantalla.----------------------------//

void leerVariables(int temp, int hum) {
  lcd.setCursor(0, 1);                                                  // Ubicamos el cursor en la primera posición(columna:0) de la segunda línea(fila:1)
  lcd.print("Temp: ");                         // Escribimos el número de segundos trascurridos
  lcd.print(temp);
  lcd.print((char)223);
  lcd.print("C Hum: ");                         // Escribimos el número de segundos trascurridos
  lcd.print(hum);
  lcd.write("%"); 
}

//-----------------------------------Método para cambiar o no el mensaje de encabezado.-------------------------------//

void encabezadoLcd(boolean riego) {
  if (!riego) {
    encabezadoLcdEstatico();
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Regando cultivo...");
  }
}

//------------------------------------------Método para limpiar la pantalla.-----------------------------------------//

void limpiarLcd() {
  lcd.clear();
}
