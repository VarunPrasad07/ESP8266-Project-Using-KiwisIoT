#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <KiwisIoT.h>

#define GAS_SENSOR A0
#define RELAY D7
#define DOUT D5
#define CLK D6

HX711 scale;

SoftwareSerial gsm(D3, D4);   // RX,TX
LiquidCrystal_I2C lcd(0x27,16,2);

// WIFI
const char* ssid  = "Varun";
const char* pass  = "12345678";
const char* topic = "dash_1773636809838";

KiwisIoT kiwisiot(ssid, pass, topic);

String phoneNumber = "+919952706210";

float calibration_factor = 420;

int gasValue;
float weight;

int gasThreshold = 400;

bool gasSmsSent = false;   // ONLY for gas alert

// SMS FUNCTION
void sendSMS(String message)
{
  gsm.println("AT");
  delay(1000);

  gsm.println("AT+CMGF=1");
  delay(1000);

  gsm.print("AT+CMGS=\"");
  gsm.print(phoneNumber);
  gsm.println("\"");
  delay(1000);

  gsm.print(message);
  delay(500);

  gsm.write(26);
  delay(5000);
}

void setup()
{
  Serial.begin(115200);
  gsm.begin(115200);

  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);

  lcd.init();
  lcd.backlight();

  kiwisiot.begin();

  scale.begin(DOUT, CLK);
  scale.set_scale(calibration_factor);
  scale.tare();

  lcd.setCursor(0,0);
  lcd.print("Gas Monitor");
  delay(2000);
  lcd.clear();
}

void loop()
{
  kiwisiot.run();

  gasValue = analogRead(GAS_SENSOR);
  weight = scale.get_units(5);

  // 🔥 GAS LEAKAGE CONTROL
  if(gasValue > gasThreshold)
  {
    digitalWrite(RELAY, HIGH);

    if(!gasSmsSent)
    {
      sendSMS("⚠️ Gas Leakage Detected!");
      gasSmsSent = true;
    }
  }
  else
  {
    digitalWrite(RELAY, LOW);
    gasSmsSent = false; // reset when safe
  }

  // SERIAL
  Serial.print("Gas: ");
  Serial.print(gasValue);
  Serial.print(" Weight: ");
  Serial.println(weight);

  // LCD DISPLAY
  lcd.setCursor(0,0);
  lcd.print("Gas:");
  lcd.print(gasValue);
  lcd.print("   ");

  lcd.setCursor(0,1);
  lcd.print("Wt:");
  lcd.print(weight);
  lcd.print("kg ");

  // IoT SEND
  kiwisiot.send("0", gasValue);
  kiwisiot.send("1", weight);

  delay(1000);
}
