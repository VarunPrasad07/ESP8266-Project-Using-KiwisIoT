#include <ESP8266WiFi.h>
#include <Wire.h>
#include <KiwisIoT.h>
#include "MAX30105.h"
#include "heartRate.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

// -------- WIFI --------
const char* ssid  = "health";
const char* pass  = "123412345";
const char* topic = "dash_1776318959410";

KiwisIoT kiwisiot(ssid, pass, topic);

// -------- GSM --------
SoftwareSerial gsm(D7, D8);

// -------- ECG --------
#define ECG_PIN A0
#define LO_PLUS D5
#define LO_MINUS D6
int ecgPrev = 0;

// -------- MAX30102 --------
MAX30105 particleSensor;

#define RATE_SIZE 4
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;

float bpm = 0;
int avgBPM = 0;
float spo2 = 0;

// -------- TEMP --------
#define ONE_WIRE_BUS D4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float temperatureC = 0;

// -------- TIMERS --------
unsigned long lastECG = 0;
unsigned long lastTemp = 0;
unsigned long lastSend = 0;

// ================= SMS =================
void sendSMS(String msg) {
  gsm.println("AT+CMGF=1");
  delay(300);
  gsm.println("AT+CMGS=\"+918681058703\"");
  delay(300);
  gsm.print(msg);
  delay(300);
  gsm.write(26);
}

// ================= KIWI =================
void onControl(String ch, String val) {
  if (ch == "4" && val == "alert") {
    sendSMS("ALERT! Patient condition abnormal.");
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  pinMode(LO_PLUS, INPUT);
  pinMode(LO_MINUS, INPUT);

  gsm.begin(9600);

  Wire.begin(D2, D1);

  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD)) {
    Serial.println("MAX30102 not found");
    while (1);
  }

  // Proper config (important)
  particleSensor.setup(60, 4, 2, 100, 411, 4096);
  particleSensor.setPulseAmplitudeIR(0x3F);
  particleSensor.setPulseAmplitudeRed(0x3F);

  sensors.begin();

  kiwisiot.begin();
  kiwisiot.onReceive(onControl);

  Serial.println("System Ready");
}

// ================= LOOP =================
void loop() {

  kiwisiot.run();

  // ===== MAX30102 (HIGH PRIORITY) =====
  long irValue = particleSensor.getIR();
  long redValue = particleSensor.getRed();

  bool fingerPresent = (irValue > 15000);

  if (fingerPresent) {

    if (checkForBeat(irValue)) {
      long delta = millis() - lastBeat;
      lastBeat = millis();

      bpm = 60 / (delta / 1000.0);

      if (bpm > 40 && bpm < 180) {
        rates[rateSpot++] = (byte)bpm;
        rateSpot %= RATE_SIZE;

        int sum = 0;
        for (byte i = 0; i < RATE_SIZE; i++) sum += rates[i];
        avgBPM = sum / RATE_SIZE;
      }
    }

    // Simple SpO2 (approx)
    float ratio = (float)redValue / (float)irValue;
    spo2 = 110 - (25 * ratio);

    if (spo2 > 100) spo2 = 100;
    if (spo2 < 0) spo2 = 0;

  } else {
    avgBPM = 0;
    spo2 = 0;
  }

  // ===== SEND (LOW RATE) =====
  if (millis() - lastSend > 200) {
    lastSend = millis();

    kiwisiot.send("2", avgBPM);
    kiwisiot.send("3", spo2);

    Serial.print("IR: "); Serial.print(irValue);
    Serial.print(" | BPM: "); Serial.print(avgBPM);
    Serial.print(" | SpO2: "); Serial.println(spo2);
  }

  // ===== ECG =====
  if (millis() - lastECG > 10) {
    lastECG = millis();

    if (digitalRead(LO_PLUS) || digitalRead(LO_MINUS)) {
      kiwisiot.send("1", 0);
    } else {
      int raw = analogRead(ECG_PIN);
      ecgPrev = (ecgPrev * 0.8) + (raw * 0.2);
      kiwisiot.send("1", ecgPrev);
    }
  }

  // ===== TEMP =====
  if (millis() - lastTemp > 1000) {
    lastTemp = millis();

    sensors.requestTemperatures();
    temperatureC = sensors.getTempCByIndex(0);

    if (temperatureC == -127) temperatureC = 0;

    kiwisiot.send("5", temperatureC);
  }
}
