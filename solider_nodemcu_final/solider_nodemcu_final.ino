#include <KiwisIoT.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

const char* ssid  = "IOT";
const char* pass  = "12341234";
const char* topic = "dash_1771915522197";

KiwisIoT kiwisiot(ssid, pass, topic);

// -------- GPS --------
static const int RXPin = D6;  // GPS TX
static const int TXPin = D5;  // GPS RX
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;
SoftwareSerial gpsSerial(RXPin, TXPin);

// -------- UNO DATA --------
#define SENSOR_COUNT 4   // temp, hr, gsr, status

String data = "";
float values[SENSOR_COUNT];

// -------- GPS VARIABLES --------
float latitude = 10.9291;
float longitude = 78.7382;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(GPSBaud);
  kiwisiot.begin();
}

void loop() {

  kiwisiot.run();

  // -------- READ UNO SERIAL --------
  if (Serial.available()) {

    data = Serial.readStringUntil('\n');
    data.trim();

    int lastPos = 0;

    for (int i = 0; i < SENSOR_COUNT; i++) {

      int commaPos = data.indexOf(',', lastPos);
      if (commaPos == -1) commaPos = data.length();

      values[i] = data.substring(lastPos, commaPos).toFloat();
      lastPos = commaPos + 1;
    }

    // Send UNO data (Widgets 1–4)
    for (int i = 0; i < SENSOR_COUNT; i++) {

      char widgetID[4];
      itoa(i + 1, widgetID, 10);

      kiwisiot.send(widgetID, values[i]);

      Serial.print("Widget ");
      Serial.print(widgetID);
      Serial.print(" = ");
      Serial.println(values[i]);
      String gpsData = String(latitude, 6) + "," + String(longitude, 6);

  kiwisiot.send("12", gpsData);

  Serial.print("GPS: ");
  Serial.println(gpsData);
    }
  }

  // -------- READ GPS --------
  while (gpsSerial.available() > 0) {
  gps.encode(gpsSerial.read());
}

if (gps.location.isValid()) {

  latitude = gps.location.lat();
  longitude = gps.location.lng();

  String gpsData = String(latitude, 6) + "," + String(longitude, 6);

  kiwisiot.send("12", gpsData);

  Serial.print("GPS: ");
  Serial.println(gpsData);
}
}
