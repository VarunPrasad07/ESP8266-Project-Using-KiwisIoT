/*
 * Kiwistron
 * KiwisIoT (Web-based IoT platform for sending & receiving data)
 * Author: Varun Prasad
 */
#include <KiwisIoT.h>   // Library for KiwisIoT communication

const char* ssid  = "Robots";        
const char* pass  = "12345678";   
const char* topic = "dash_1784798595235"; // Dashboard topic ID

KiwisIoT kiwisiot(ssid, pass, topic); // IoT object

#define SENSOR_PIN D5  

int state = 0;          

void setup() {
  Serial.begin(9600);        
  pinMode(SENSOR_PIN, INPUT);
  kiwisiot.begin();          // Start IoT connection
}

void loop() {
  kiwisiot.run();            // Maintain connection

  state = digitalRead(SENSOR_PIN);

  kiwisiot.send("1", state); // Send to widget ID "1"

  Serial.print("Digital State = ");
  Serial.println(state);    

  delay(1000); 
}
