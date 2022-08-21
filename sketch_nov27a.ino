// Referências
// https://iotdesignpro.com/projects/how-to-connect-esp32-with-ibm-watson-cloud-platform
//https://www.arduino.cc/reference/en/libraries/pubsubclient/

#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <MFRC522.h> //biblioteca responsável pela comunicação com o módulo RFID-RC522
#include <SPI.h> //biblioteca para comunicação do barramento SPI

#define SS_PIN    21
#define RST_PIN   22

#define SIZE_BUFFER     18
#define MAX_SIZE_BLOCK  16

#include <MFRC522.h> //biblioteca responsável pela comunicação com o módulo RFID-RC522
#include <SPI.h> //biblioteca para comunicação do barramento SPI

#define SS_PIN    21
#define RST_PIN   22

#define SIZE_BUFFER     18
#define MAX_SIZE_BLOCK  16

const int vermelho = 5;
const int verde = 4;

String conteudo = "";

//----Configuração de rede do Wi-fi-------//
const char* ssid = "Bombrasil_Quarto";
const char* password = "5614paty";

// Configuração Broker IOT
char server[] = "mqtt.eclipseprojects.io";
char pubTopic1[] = "teste/rfid";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// Variais de contagem de eventos
int entrada = 0;
int saida = 0;
String codigo = "";

#define TEMPO_DEBOUNCE 10 //ms
int contador_acionamentos = 0;
unsigned long timestamp_ultimo_acionamento = 0;


//esse objeto 'chave' é utilizado para autenticação
MFRC522::MIFARE_Key key;
//código de status de retorno da autenticação
MFRC522::StatusCode status;

// Definicoes pino modulo RC522
MFRC522 mfrc522(SS_PIN, RST_PIN);


void leituraDados()
{
  //imprime os detalhes tecnicos do cartão/tag
  //mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));

  //---------------------
  byte letra;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : ""));
    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

}
  void setup() {
    // Inicia a serial
    Serial.begin(115200);
    SPI.begin(); // Init SPI bus

    //Inicia os Leds:
    pinMode (vermelho, OUTPUT);
    pinMode (verde, OUTPUT);
    
    // Inicia MFRC522
    mfrc522.PCD_Init();

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
      digitalWrite (vermelho, HIGH);
      digitalWrite (verde, HIGH);

      delay(1000);
      digitalWrite (vermelho, LOW);
      digitalWrite (verde, LOW);
    }
    // Congiguração da contagem de peças de entrada
  }

  long lastMsg = 0;


  void loop()
  {
    // Aguarda a aproximacao do cartao
    if ( ! mfrc522.PICC_IsNewCardPresent())
    {
      return;
    }
    // Seleciona um dos cartoes
    if ( ! mfrc522.PICC_ReadCardSerial())
    {
      return;
    }

    //chama o menu e recupera a opção desejada
    leituraDados();
    // instrui o PICC quando no estado ACTIVE a ir para um estado de "parada"
    mfrc522.PICC_HaltA();
    // "stop" a encriptação do PCD, deve ser chamado após a comunicação com autenticação, caso contrário novas comunicações não poderão ser iniciadas
    mfrc522.PCD_StopCrypto1();



    client.loop();
    long now = millis();
    if (now - lastMsg > 3000) {
      lastMsg = now;
      //-------Emulado--------------/
      saida += 0;
      // ----------------Configuração do pacote de envio---------------------------
      //String payload = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
      String payload = "{\"Name\":\"" "001" "\"";
      payload += ",\"Entrada\":";
      payload += String(conteudo);
      payload += "}";

      conteudo = "";
      // --------------------------------------------------------------------------
      // Envio do pacote
      Serial.print("Sending payload: ");
      Serial.println(payload);
      if (client.publish("teste/teste1000", (char*) payload.c_str())) {
        Serial.println("Publish ok");
        digitalWrite (verde, HIGH);
        delay(2000);
        digitalWrite(verde, LOW);
        
      } else {
        Serial.println("Publish failed");
        digitalWrite(vermelho, HIGH);
        delay(2000);
        digitalWrite(vermelho, LOW);
      }
    }
  }
