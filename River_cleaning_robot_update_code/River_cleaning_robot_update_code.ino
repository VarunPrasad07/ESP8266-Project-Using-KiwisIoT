#include<Servo.h>
#define IN1 2
#define IN2 3
#define IN3 4
#define IN4 5
#define TRIG 7
#define ECHO 8

Servo myServo;
void setup(){
 Serial.begin(115200);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
pinMode(IN3,OUTPUT);
pinMode(IN4,OUTPUT);
pinMode(TRIG,OUTPUT);
pinMode(ECHO,INPUT);

  myServo.attach(6);
  myServo.write(90);
}
void motorForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}

void motorReverse() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
}

void motorRight() {
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void motorLeft() {
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void motorStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void loop() {


    if (cmd == "forward") motorForward();
    else if (cmd == "back") motorReverse();
    else if (cmd == "right") motorRight();
    else if (cmd == "left") motorLeft();
    else motorStop();
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH);
  long distance = duration * 0.034 / 2;

  if (distance > 0 && distance < 10) {
    motorStop();
    myServo.write(0);
  } else {
    myServo.write(90);
  }

  delay(1000);
}
    
  
