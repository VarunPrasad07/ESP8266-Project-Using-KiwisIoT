/*
 * Kiwistron
 * KiwisIoT (Web-based IoT platform for sending & receiving data)
 * Author: Varun Prasad
 */

#include <KiwisIoT.h>   // IoT communication library
#include <DHT.h>        // DHT sensor library

const char* ssid  = "Robots";        
const char* pass  = "12345678";  
const char* topic = "dash_1784798595235";// Dashboard topic

KiwisIoT kiwisiot(ssid, pass, topic); // IoT object

#define DHTPIN D4        
#define DHTTYPE DHT11    

DHT dht(DHTPIN, DHTTYPE); 

float temperature = 0; 
float humidity = 0;    

void setup() {
  Serial.begin(9600);  
  kiwisiot.begin();    // Start IoT connection  
  dht.begin();         // Initialize DHT sensor
}

void loop() {
  kiwisiot.run();      // Maintain connection

  humidity = dht.readHumidity();       // Read humidity (%)
  temperature = dht.readTemperature(); // Read temperature (°C)

  // Check if reading failed
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("DHT11 Read Failed");
    return;
  }

  kiwisiot.send("1", temperature); // 1 is channel number
  kiwisiot.send("2", humidity);   // 2 is channel number

  Serial.print("Temp = ");
  Serial.print(temperature);
  Serial.print(" °C | Humidity = ");
  Serial.println(humidity);

  delay(500); 
}
