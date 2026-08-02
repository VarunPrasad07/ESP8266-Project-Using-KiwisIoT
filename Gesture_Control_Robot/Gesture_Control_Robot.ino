#include <KiwisIoT.h>

const char* ssid  = "IOT";
const char* pass  = "12345678";
const char* topic = "dash_1772000574792";

KiwisIoT kiwisiot(ssid, pass, topic);

#define IN1 D1
#define IN2 D2
#define IN3 D3
#define IN4 D4

void setup()
{
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
kiwisiot.begin();
}

void loop()
{
  kiwisiot.run();

}
void onControl(String ch, String val){
  Serial.println(val);
  val.trim();
  if (ch=="0"){
    if (val=="F"){
      forward();

    }
    else if (val=="B"){
      backward();
    
    }
    else if (val=="R"){
      right();

    }
    else if (val =="L"){
      left();
    }
    else {
      stopRobot();
    }
  }
}
void forward()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backward()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void left()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void right()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopRobot()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
