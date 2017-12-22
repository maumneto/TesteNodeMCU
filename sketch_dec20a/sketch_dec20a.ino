#include "DHT.h"
#include <Adafruit_Sensor.h>
#include <PubSubClient.h>

#define DHTPIN 2     
#define DHTTYPE DHT11   // DHT 11

//DHT dht(DHTPIN, DHTTYPE);

void setup() {
  //Serial.begin(9600);
  Serial.println("Inicializando o hardware...");
  //dht.begin();
  // Define o pino 13 como saida
  pinMode(D7, OUTPUT);
}
 
void loop() {
  digitalWrite(D7, HIGH);   // Acende o Led
  delay(3000);              // Aguarda 1 segundo
  digitalWrite(D7, LOW);    // Apaga o Led
  delay(1000);              // Aguarda 1 segundo
}             
