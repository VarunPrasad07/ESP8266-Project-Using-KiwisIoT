#include <ESP8266WiFi.h>
#include <KiwisIoT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// WIFI
const char* ssid  = "gas";
const char* pass  = "12345678";
const char* topic = "dash_1773382972250";

KiwisIoT kiwisiot(ssid, pass, topic);

// LCD
LiquidCrystal_I2C lcd(0x27,16,2);

// PINS
#define GAS_PIN A0
#define LED_PIN D6
#define BUZZER_PIN D7

int gasThreshold = 52;

unsigned long lastSend = 0;
const long interval = 2000;

void setup()
{
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  lcd.init();
  lcd.backlight();

  kiwisiot.begin();

  lcd.setCursor(0,0);
  lcd.print("Gas Monitor");
  delay(2000);
  lcd.clear();

  Serial.println("Gas Leakage System Started");
}

void loop()
{
  kiwisiot.run();

  unsigned long now = millis();

  if(now - lastSend > interval)
  {
    lastSend = now;

    int gasValue = analogRead(GAS_PIN);

    Serial.print("Gas Value: ");
    Serial.println(gasValue);

    // Send to IoT
    kiwisiot.send("0", gasValue);

    lcd.setCursor(0,0);
    lcd.print("Gas:");
    lcd.print(gasValue);
    lcd.print("   ");

    if(gasValue > gasThreshold)
    {
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);

      lcd.setCursor(0,1);
      lcd.print("GAS LEAKAGE!  ");

      kiwisiot.send("4",1);
      kiwisiot.send("5",1);

      Serial.println("Gas Detected");
    }
    else
    {
      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);

      lcd.setCursor(0,1);
      lcd.print("Normal Air    ");

      kiwisiot.send("4",0);
      kiwisiot.send("5",0);

      Serial.println("Normal Air");
    }
  }
}
