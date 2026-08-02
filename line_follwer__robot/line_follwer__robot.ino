// ==================== LINE FOLLOWER ROBOT ====================
// IR Sensor-based line following
// Three-sensor configuration with basic motor control
// ===========================================================

// ===================== PIN DEFINITIONS =====================
// Line Follower Sensors
#define LEFT_SENSOR   8
#define MIDDLE_SENSOR 7
#define RIGHT_SENSOR  9

// Motor Control
#define ENA 6  // PWM - Left motor speed
#define ENB 11    // PWM - Right motor speed
#define IN1 2     // Left motor direction
#define IN2 3     // Left motor direction
#define IN3 4     // Right motor direction
#define IN4 5     // Right motor direction

// ===================== CONSTANTS =====================
// Motor speeds
const int LINE_FOLLOW_SPEED = 50;
const int TURN_SPEED = 100;

// ===================== SETUP =====================
void setup() {
  // Line Follower pins
  pinMode(LEFT_SENSOR, INPUT);
  pinMode(MIDDLE_SENSOR, INPUT);
  pinMode(RIGHT_SENSOR, INPUT);

  // Motor control pins
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.begin(9600);
  delay(1000);
  Serial.println("=== Line Follower Robot Started ===");
}

// ===================== MAIN LOOP =====================
void loop() {
  lineFollowerMode();
}

// ===================== LINE FOLLOWER MODE =====================
// Dedicated line follower functions

int readLineFollowerSensors() {
  int L = digitalRead(LEFT_SENSOR);
  int M = digitalRead(MIDDLE_SENSOR);
  int R = digitalRead(RIGHT_SENSOR);
  return (L << 2) | (M << 1) | R;  // Return as 3-bit value
}

void printLineFollowerSensorStatus() {
  int L = digitalRead(LEFT_SENSOR);
  int M = digitalRead(MIDDLE_SENSOR);
  int R = digitalRead(RIGHT_SENSOR);
  Serial.print("Line Sensors: L=");
  Serial.print(L);
  Serial.print(" M=");
  Serial.print(M);
  Serial.print(" R=");
  Serial.println(R);
}

void lineFollowerMode() {
  int sensorState = readLineFollowerSensors();
  printLineFollowerSensorStatus();

  switch (sensorState) {
    // Middle sensor on black (straight)
    case 0b101:  // L=0, M=1, R=0
      motorForward();
      break;

    // Left sensor on black (sharp left)
    case 0b011:  // L=1, M=0, R=0
      motorTurnLeft();
      break;

    // Right sensor on black (sharp right)
    case 0b110:  // L=0, M=0, R=1
      motorTurnRight();
      break;

    // Left + Middle on black (moderate left)
    case 0b001:  // L=1, M=1, R=0
      motorTurnLeft();
      break;

    // Right + Middle on black (moderate right)
    case 0b100:  // L=0, M=1, R=1
      motorTurnRight();
      break;

    // All black (continue forward)
    case 0b000:  // L=1, M=1, R=1
      motorForward();
      break;

    // All white - line lost
    case 0b111:  // L=0, M=0, R=0
    default:
      motorStop();
      Serial.println(">>> LINE LOST <<<");
      break;
  }

  delay(10);
}


// ===================== MOTOR CONTROL =====================
// Motor functions for line following

void setMotors(int leftDir, int rightDir, int speed) {
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  digitalWrite(IN1, leftDir > 0 ? HIGH : LOW);
  digitalWrite(IN2, leftDir < 0 ? HIGH : LOW);
  digitalWrite(IN3, rightDir > 0 ? HIGH : LOW);
  digitalWrite(IN4, rightDir < 0 ? HIGH : LOW);
}

void motorForward() {
  setMotors(1, 1, LINE_FOLLOW_SPEED);
}

void motorTurnLeft() {
  setMotors(-1, 1, TURN_SPEED);
}

void motorTurnRight() {
  setMotors(1, -1, TURN_SPEED);
}

void motorStop() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
