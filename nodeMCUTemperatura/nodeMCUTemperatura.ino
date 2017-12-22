/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *    TESTE DO NODEMCU + DHT11
 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

// Bibliotecas necessarias (instalar caso não tenha)
#include "DHT.h"
#include <Adafruit_Sensor.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Macros utilizadas 
#define DHTPIN D4
#define PinBlue D5
#define PinRed D6
#define DHTTYPE DHT11

String topic = "NodeMCU/Temperatura";
int errorConectionMQTT = -2;
int status = WL_IDLE_STATUS;

IPAddress server(192, 168, 1, 101);
const char ssid[] = "LittleGreat";
const char password[] = "grt@lifesg00d!16";
const char* mqtt_server = "broker.hivemq.com"; 

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

/* ++++++++++++++++++++++++++++++++++++++
 *                SETUP
 ++++++++++++++++++++++++++++++++++++++++ */
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600); // comunicação
  pinMode(PinBlue, OUTPUT);
  pinMode(PinRed, OUTPUT);
  
  dht.begin();
  InitWiFi();
  
  client.setServer(server,1883);
}

/* ++++++++++++++++++++++++++++++++++++++
 *                WIFI SETUP
 ++++++++++++++++++++++++++++++++++++++++ */
void InitWiFi(){
    //Serial.begin(9600);
    //WiFi.init(&Serial1);
    if (WiFi.status() == WL_NO_SHIELD){
      Serial.println("modulo WiFi não conectado");
      while(true);
      }
      Serial.println("Conectando ao AP ...");
      while ( status != WL_CONNECTED) {
          Serial.print("Tentando conectar ao WPA SSID: ");
          Serial.println(ssid);
          status = WiFi.begin(ssid, password);
          delay(500);
       }
       Serial.println("Conectando ao AP");
}

/* ++++++++++++++++++++++++++++++++++++++
 *                SENSOR DATA
 ++++++++++++++++++++++++++++++++++++++++ */
 void getSensorData(){
  //Serial.println("Inicializando o sensor!");
  digitalWrite(PinRed, HIGH);
  float umid = dht.readHumidity();
  float temp = dht.readTemperature();

  // Verifica se existe erro na leitura do sensor de temperatura!
  if (isnan(umid) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
    digitalWrite(PinRed, LOW);
    digitalWrite(PinBlue, HIGH);
    return;
  }

   // convertendo para string
   String temperatura = String(temp);
   String umidade = String(umid);

   // debugando mensagens
   Serial.print( "Enviando os dados de humidade e temperatura : [" );
   Serial.print(umidade);
   Serial.print(" ] [ ");
   Serial.print(temperatura);
   Serial.print( " ] -> " );
      
   // JSON
   String payload = " { ";
   payload += "\"topico\":\""; payload += topic;
   payload += "\" , ";
   payload += "\"Umidade\":"; payload += umidade;
   payload += " , ";
   payload += "\"Temperatura\":"; payload += temperatura;
   payload += " } ";

   // Send payload
   char attributes[1000];
   payload.toCharArray( attributes,1000);
   client.publish("NodeMCU/Temperatura", attributes);
   Serial.println( attributes );
   digitalWrite(PinRed, HIGH);
   digitalWrite(PinBlue, LOW);
   delay(4000);
}

/* ++++++++++++++++++++++++++++++++++++++
 *                RECONNECT
 ++++++++++++++++++++++++++++++++++++++++ */
void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conectar com o Broker ...");
    if (client.connect("esp8266Client")) {
      Serial.println("Conectado!");
      } else {
      //
      char attributes2[100];
      String payload = " { ";
      payload += "\"error\":"; payload += errorConectionMQTT;
      payload += " } ";
      payload.toCharArray( attributes2, 100 );
      client.publish( "NodeMCU/Temperatura", attributes2 );
      //      
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando em 5 segundos ");
      delay(5000);
    }
  }
}

void loop() {
  status = WiFi.status();
  if (status != WL_CONNECTED){
      while(status != WL_CONNECTED){
          Serial.print("Tentando conexão para WPA SSID: ");
          Serial.println(ssid);
          // conectando com a rede
          status = WiFi.begin(ssid,password);
          delay(500);
        }
        Serial.println("Conectado ao AP");
    }
    // tentando se reconectar
    if (!client.connected()){
        reconnect();
      }
      getSensorData();
      delay(2000);
      client.loop();
}

