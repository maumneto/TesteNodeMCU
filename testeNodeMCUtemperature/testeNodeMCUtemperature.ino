/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *    TESTE DO NODEMCU + DHT11
 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
#include "DHT.h"
#include <Adafruit_Sensor.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define DHTPIN D4     
#define DHTTYPE DHT11   // DHT 11

const char* ssid = "LittleGreat";
const char* password = "grt@lifesg00d!16";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

int errorMQTT = -2;
String topic = "NODEMCU/TemperaturaMCU";

/* ++++++++++++++++++++++++++++++++++++++
 *                SETUP
 ++++++++++++++++++++++++++++++++++++++++ */
void setup() {
  Serial1.begin(9600);
  Serial.begin(9600);
  
  initWiFi();
  
  client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
  
  dht.begin();
  
  pinMode(D7, OUTPUT);
  pinMode(D2, OUTPUT);
}

/* ++++++++++++++++++++++++++++++++++++++
 *                WIFI SETUP
 ++++++++++++++++++++++++++++++++++++++++ */
void initWiFi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  if (WiFi.status() != WL_CONNECTED){
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print("Tentando se conectar ao SSID:" );
      Serial.println(ssid);
      WiFi.begin(ssid, password);
      delay(500);
    }
  Serial.println("Conectado ao AP");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  }
}
/* ++++++++++++++++++++++++++++++++++++++
 *                SENSOR DATA
 ++++++++++++++++++++++++++++++++++++++++ */
void getSensorData(){
  Serial.println("Inicializando o sensor...");
  digitalWrite(D2, HIGH);
  float umid = dht.readHumidity();
  float temp = dht.readTemperature();

  // Verifica se existe erro na leitura do sensor de temperatura!
  if (isnan(umid) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
    digitalWrite(D7, HIGH);
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
   payload += "\" , ";
   payload += "\"Temperatura\":"; payload += temperatura;
   payload += " } ";

   // Send payload
   char attributes[100];
   payload.toCharArray( attributes, 100 );
   client.publish("NODEMCU/TemperaturaMCU", attributes );
   Serial.println( attributes );
   digitalWrite(D7, HIGH);
   delay(4000);
}

/* ++++++++++++++++++++++++++++++++++++++
 *               CALLBACK
 ++++++++++++++++++++++++++++++++++++++++ */
//void callback(char* topic, byte* payload, unsigned int length) {
//  Serial.print("Message arrived [");
//  Serial.print(topic);
//  Serial.print("] ");
//  for (int i = 0; i < length; i++) {
//    Serial1.print((char)payload[i]);
//  }
//  Serial1.println();
//}

/* ++++++++++++++++++++++++++++++++++++++
 *                RECONNECT
 ++++++++++++++++++++++++++++++++++++++++ */
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      String payload;
      char attributes2[100];
      payload.toCharArray( attributes2, 100 );
      client.publish("NODEMCU/TemperaturaMCU", attributes2 );
      //client.subscribe("inTopic");
      } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

/* ++++++++++++++++++++++++++++++++++++++
 *                LOOP
 ++++++++++++++++++++++++++++++++++++++++ */
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  getSensorData();
  delay(1000);
  client.loop();

//  long now = millis();
//  if (now - lastMsg > 2000) {
//    lastMsg = now;
//    ++value;
//    snprintf (msg, 75, "hello world #%ld", value);
//    Serial.print("Publish message: ");
//    Serial.println(msg);
//    client.publish("outTopic", msg);
}

