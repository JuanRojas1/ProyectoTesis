

//-----------------------------------------Librerias-------------------------------------------//

#include <ESP8266wifi.h>
#include <strings_en.h>
#include <WiFiManager.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <Time.h>
//#include <ESP_WiFiManager.h>
//#include <ESP_WiFiManager.hpp>
//#include <ESP8266wifi.h>

//-----------------------------Clase para la conexión a Internet-------------------------------//

  class ConexionEthernet{

  public: 
  void conexion(){      
      WiFiManager wifiManager;                                    //Creamos una instancia de la clase WiFiManager.   
      //wifiManager.resetSettings();                            //Descomentar para resetear configuracion.  (se sube el código con la linea descomentada y luego se comenta y se vuelve a subir)
  
      if(!wifiManager.autoConnect("Irrigation_Sistem")){        //creamos AP(Punto de Acceso) y portal cautivo.
      Serial.println("Falló la conexión");
      //ESP.reset();
      delay(1000);
    }
      Serial.println("Ya estas conectado");
  } 
    };
