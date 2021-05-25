
/**
 * 
 * NOME: PEDRO VICTOR DINI SILVA DE PAULA
 * TIA: 31857345 
 * MATÉRIA: OBJETOS INTELIGENTES CONECTADOS 
 * TURMA: 05k
 * PROFESSOR: DR.
 * PROJETO: DOMÓTICA - AUTOMAÇÃO RESIDENCIAL (COZINHA)
 * 
 * 
   * ESTE PROJETO É DE AUTORIA PRÓPRIA. QUALQUER CÓPIA PRECISA SER REFERENCIADA. 
 * 
 * 
 */

 
 
// Inclusão de Bibliotecas 
#include <Servo.h> // Servo 
#include "DHT.h" // Sensor DHT11
#include <ESP8266WiFi.h>  // Modulo Wi-FI
#include <PubSubClient.h>  // MQTT 

//WiFi - configuração 
const char* SSID = "Kingdom";            
const char* PASSWORD = "hamburguer10";   
WiFiClient wifiClient;

//MQTT Server
const char* BROKER_MQTT = "iot.eclipse.org"; 
int BROKER_PORT = 1883;                  

#define ID_MQTT  "NODEMCU01"            
#define TOPIC_PUBLISH "COMP001" //SENSOR DE PRESENÇA
#define TOPIC_PUBLISH "COMP002" // LED
#define TOPIC_PUBLISH "COMP003" // SENSOR DHT
#define TOPIC_PUBLISH "COMP004" // SERVO


PubSubClient MQTT(wifiClient);       

//Declaração das Funções
void mantemConexoes();  
void conectaWiFi();     
void conectaMQTT();     
void enviaPacote();   

//------------------------------------------------------------------------------------------------------------------------------
  
// Declaração de setup DHT11 - Sensor de temperatura 
#define DHTPIN D4 
#define DHTTYPE DHT11 

DHT dht (DHTPIN, DHTTYPE); 

// Declaração de setup Sensor de Movimento e presença PIR 

#define PIN_SENSOR D1
#define PIN_LED D3

// Declaração de setup Servo SG92R 
#define SERVO D0

  Servo servo1

  void setup() {

  // SETUP MQTT 
  conectaWiFi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);  
    
    // SETUP - Sensor de Movimento de Presença PIR 
   // Configuração do Sensor PIR
   pinMode(PIN_SENSOR,INPUT);

   // Cinfiguração do LED  
   pinMode(PIN_LED,OUTPUT); 

  // -------------------------------------------- 

  // SETUP - Sensor de Umidade e Temperatura 
  // Inicialização Serial e DHT11 
  Serial.begin(9600);
  dht.begin(); 


 // ----------------------------------------------

 // SETUP - Micro Servo SG92R

      servo1.attach(SERVO); 
 
  
}


void loop() {

  int pos; // Declaração para o Servo SG92R

  mantemConexoes();
  MQTT.loop();
  
  //Leitura do sensor de presença 
  int sinal_sensor = digitalRead(PIN_SENSOR);

  //Condição de movimento detectado
  if(sinal_sensor == HIGH){
    //Aciona o LED
    digitalWrite(PIN_LED, HIGH);
    delay(1000);
    
  }
  else{
    //desliga o lED 
    digitalWrite(PIN_LED, LOW); 
  }



  //Leitura de Umidade
  float umidadeSensor = dht.readHumidity();

  //Leitura de Temperatura
  float temperaturaSensor = dht.readTemperature(); 

  //Verificação de erros de leitura 
  if (isnan(umidadeSensor) || isnan(temperaturaSensor)){
    Serial.println("Leitura com erro, verifique!");
    return; 
  }

  if (temperaturaSensor > "25"){
      for (pos = 0; pos <= 290; pos += 1) { 
    
        servo1.write(pos);             
        delay(12);                       
  }
      for (pos = 290; pos >= 0; pos -= 1) { 
        servo1.write(pos);              
        delay(12);   

    for (pos = 0; pos <= 180; pos += 1) { 
      servo1.write(pos);             
      delay(15);                       
  }
    for (pos = 180; pos >= 0; pos -= 1) { 
      servo1.write(pos);              
      delay(15);                       
  }
    
  }

  //Apresentação de Dados no Serial 
  Serial.print("Umidade Local: ");
  Serial.print(umidadeSensor);
  Serial.println("%\t"); 
  Serial.print("Temperatura Local: ");
  Serial.print(temperaturaSensor);
  Serial.println("Celsius");   

  delay(2000); 
  

}

void mantemConexoes() {
    if (!MQTT.connected()) {
       conectaMQTT(); 
    }
    
    conectaWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

void conectaWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
     return;
  }
        
  Serial.print("Conectando-se na rede: ");
  Serial.print(SSID);
  Serial.println("  Aguarde!");

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI  
  while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
  }
  

  Serial.print("SUCESSO, CONECTADO! ");
  Serial.print(SSID);  
  Serial.print("  IP obtido: ");
  Serial.println(WiFi.localIP()); 
}

void conectaMQTT() { 
    while (!MQTT.connected()) {
        if (MQTT.connect(ID_MQTT)) {
        } 
        else {
            delay(1000);
        }
    }
}
