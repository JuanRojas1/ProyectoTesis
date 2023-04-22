

//-----------------------------------------Librerias-------------------------------------------//

      #include <ESP8266HTTPClient.h>                                                                                                           //Librería para la conexión a la red en modo cliente.
      
//----------------------Clase para realizar peticion Http por el metodo post--------------------//

class PeticionHttp {

  public:
    void peticionPost(String urlServidor, int tempAmbiente, int humTerreno, int humAmbiente, String estadoRiego, int tiempoRiego, String uuid) {
    HTTPClient http;
    WiFiClient client;
    http.begin(client, urlServidor);
    http.addHeader("Content-Type","application/x-www-form-urlencoded");                                                                                //Esta linea no se modifica.
    String postData = "tempAmbiente="+String(tempAmbiente)+"&humTerreno="+String(humTerreno)+
                      "&humAmbiente="+ String(humAmbiente)+"&phTerreno=0"+"&estadoRiego="+estadoRiego+"&tiempoRiego="+String(tiempoRiego)
                      +"&uuid="+(uuid) ;
    int httpCode = http.POST(postData);                                                                                                                //Se envia solicitud al servidor.
    if(httpCode > 0){                                                                                                                                  //Se obtiene respuesta.
      Serial.println("el servidor respondio: ");
      if(httpCode == 200){
          Serial.println(http.getString());
        }else{
          Serial.println(httpCode);
        }
      }  
    http.end(); 
    }
};
