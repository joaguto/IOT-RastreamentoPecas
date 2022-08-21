// Referências
// https://iotdesignpro.com/projects/how-to-connect-esp32-with-ibm-watson-cloud-platform
//https://www.arduino.cc/reference/en/libraries/pubsubclient/

#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

//----Configuração de rede do Wi-fi-------//
const char* ssid = "Galaxy A312F12";
const char* password = "vnbs5322";

// Configuração Broker IOT
char server[] = "mqtt.eclipseprojects.io";
char pubTopic1[] = "teste/teste1000";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// Variais de contagem de eventos
int entrada = 0;
int saida = 0;
String codigo = "";

#define TEMPO_DEBOUNCE 10 //ms
int contador_acionamentos = 0;
unsigned long timestamp_ultimo_acionamento = 0;





  // Configuração geral
  void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.print(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("WiFi connected, IP address: ");
    Serial.println(WiFi.localIP());
    client.setServer(server, 1883);
    if (!client.connected()) {
      Serial.print("Reconnecting client to ");
      Serial.println(server);
      
      while (!client.connect("ESP32")) {
        Serial.print(".");
        delay(500);
      }
      

      Serial.println("Conectado ao Banco de Dados");
    }
    // Congiguração da contagem de peças de entrada
  }

  long lastMsg = 0;

  void loop() {
    client.loop();
    long now = millis();
    if (now - lastMsg > 3000) {
      lastMsg = now;
      //-------Emulado--------------/
      entrada = contador_acionamentos;
      saida += 0;
      // ----------------Configuração do pacote de envio---------------------------
      //String payload = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
      String payload = "{\"Name\":\"" "001" "\"";
      payload += ",\"Entrada\":";
      payload += entrada;
      payload += ",\"saida\":";
      payload += saida;
      payload += "}";
      // --------------------------------------------------------------------------
      // Envio do pacote
      Serial.print("Sending payload: ");
      Serial.println(payload);
      if (client.publish("teste/teste1000", (char*) payload.c_str())) {
        Serial.println("Publish ok");
      } else {
        Serial.println("Publish failed");
      }
    }




  }
