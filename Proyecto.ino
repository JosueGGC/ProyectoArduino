#include <dhtESP32-rmt.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

#define SENSOR 33

//#define BOMBA 4
#define LED 32


#define SSID "IOT"
#define PSK "CNtec2023"
#define URL "http://172.18.7.249/"

WiFiMulti wifi;



int v_sensor;
int tempor;
String temop;


String jwt;
bool sesion;

String tiempo;
String humedad;
String temperatura;

float temperature = 0.0;
float humidity = 0.0;
String op;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(SENSOR, INPUT);
//pinMode(BOMBA, OUTPUT);
pinMode(LED, OUTPUT);
wifi.addAP(SSID, PSK);
while(wifi.run() != WL_CONNECTED);
Serial.println("Conectado");
}

void loop() {
  // put your main code here, to run repeatedly: 
consulta();   
hume();
tempe();  
if(sesion) registro();
else login();
delay(tiempo.toInt()*1000);
}

void hume(){
  v_sensor = analogRead(SENSOR);  
  Serial.println("La humedad es " + String(v_sensor));  
  tempor = map(v_sensor, 1023, 0, 0, 100);
  Serial.println("La humedad es " + String(tempor) + "%");
     
if(tempor == 0){
  temop = "Me mori x-x";
  digitalWrite(LED, HIGH);
}else if(tempor > 0 && tempor <= humedad.toInt()){
  temop = "Tengo sed";
  digitalWrite(LED, HIGH);
}else if(tempor > humedad.toInt() && tempor <= 50){
  temop = "Estoy bien";
  digitalWrite(LED, LOW);  
}else if(tempor > 50){
  temop = "Me ahogo x-x";
  digitalWrite(LED, LOW);   
}
}

void tempe(){
uint8_t error=read_dht(temperature, humidity, 25, DHT11, 7);
	if(error){
		Serial.println(error);
	}else{
		Serial.println("La temperaura es de: " + String(temperature));
		Serial.println("La humedad es de: " + String(humidity));
	}
	delay(3000);	

Serial.println("La temperatura es: " + String(temperature) + "°C");  

if(temperature == 0.00){
  op = "Me congelo x-x";
}else if(temperature > 0.00 && temperature <= 15.00){
  op = "hace frio";
}else if(temperature > 15.00 && temperature < 30.00){
  op = "Estoy bien";
}else if(temperature >= temperatura.toInt()){
  op = "Saquenme de aqui!!!";
}

}


void login(){
  HTTPClient http;
  http.begin(URL "login");
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST("{\"nombre\":\"admin\",\"contra\":\"123\"}");
  if(httpCode > 0) {
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);
            if(httpCode == 200) {
                String payload = http.getString();
                int p;
                p = payload.indexOf("}");
                jwt = payload.substring(10, p-2);
                Serial.println(jwt);
                sesion = true;                                               
                }
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }                        
        http.end();
}

void registro(){
  HTTPClient http;
  http.begin(URL "registro");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + jwt);
  int httpCode = http.POST("{\"id_Usuario\": 1,\"sensor_Uno\":\"Humedad\",\"valor_Uno\":\"" + String(tempor) + "\",\"op_Uno\":\"" + temop + "\",\"sensor_Dos\":\"Temperatura\",\"valor_Dos\":\"" + String(temperature) + "\",\"op_Dos\":\"" + op + "\"}");
  if(httpCode > 0) {
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);
            if(httpCode == 201) {
                String payload = http.getString();
                Serial.println(payload);                                     
                }
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }                        
        http.end();
}

void consulta(){
  HTTPClient http;
  http.begin(URL "control");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + jwt);
  int httpCode = http.GET();
  if(httpCode > 0) {
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);
            if(httpCode == 200) {
                String payload = http.getString();
                Serial.println(payload); 
                tiempo = buscarTiempo("h_Registro", payload);
                Serial.println(tiempo);
                humedad = buscarHumedad("tm_Riego", payload);
                Serial.println(humedad);
                temperatura = buscartemperatura("tm_Ambiente", payload);
                Serial.println(temperatura);                                         
                }
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }                        
        http.end();
}

String buscarTiempo(String clave, String json){
  int i,f;
  String valor;
  i = json.indexOf(clave);
  i = json.indexOf(":", i);
  i = json.indexOf("\"", i) + 1;
  f = json.indexOf("\"", i);
  return json.substring(i, f);
}
String buscarHumedad(String clave, String json){
  int i,f;
  String valor;
  i = json.indexOf(clave);
  i = json.indexOf(":", i);
  i = json.indexOf("\"", i) + 1;
  f = json.indexOf("\"", i);
  return json.substring(i, f);
}
String buscartemperatura(String clave, String json){
  int i,f;
  String valor;
  i = json.indexOf(clave);
  i = json.indexOf(":", i);
  i = json.indexOf("\"", i) + 1;
  f = json.indexOf("\"", i);
  return json.substring(i, f);
}

