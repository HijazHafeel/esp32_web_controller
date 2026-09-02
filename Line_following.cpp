#define IR_LEFT 19
#define IR_CENTER 21
#define IR_RIGHT 22

// Motor driver pins (L298N / TB6612FNG)
const int ENA = 25; // Left motor PWM
const int ENB = 26; // Right motor PWM
const int IN1 = 27; // Left motor DIR 1
const int IN2 = 14; // Left motor DIR 2
const int IN3 = 12; // Right motor DIR 1
const int IN4 = 13; // Right motor DIR 2

// PID Tuning Constants
float Kp = 60.0;  // Proportional gain: adjusts responsiveness to current error
float Ki = 0.0;   // Integral gain: corrects accumulated steady-state error
float Kd = 25.0;  // Derivative gain: dampens overshoot/oscillation

// PID Variables
float error = 0;
float previousError = 0;
float integral = 0;

// Base speeds
const int BASE_SPEED = 180;
const int MAX_SPEED  = 255;

void setup() {
  pinMode(IR_LEFT, INPUT);
  pinMode(IR_CENTER, INPUT);
  pinMode(IR_RIGHT, INPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Set default forward direction
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void driveMotors(int leftSpeed, int rightSpeed) {
  analogWrite(ENA, constrain(leftSpeed, 0, MAX_SPEED));
  analogWrite(ENB, constrain(rightSpeed, 0, MAX_SPEED));
}

void loop() {
  int L = digitalRead(IR_LEFT);
  int C = digitalRead(IR_CENTER);
  int R = digitalRead(IR_RIGHT);

  // Determine error state from sensor configuration
  // Negative error = line is to the left; Positive error = line is to the right
  if (L == LOW && C == HIGH && R == LOW) {
    error = 0;   // Centered on track
  } else if (L == HIGH && C == HIGH && R == LOW) {
    error = -1;  // Slightly off-center right (line shifting left)
  } else if (L == HIGH && C == LOW && R == LOW) {
    error = -2;  // Off-center right (line far left)
  } else if (L == LOW && C == HIGH && R == HIGH) {
    error = 1;   // Slightly off-center left (line shifting right)
  } else if (L == LOW && C == LOW && R == HIGH) {
    error = 2;   // Off-center left (line far right)
  } else if (L == LOW && C == LOW && R == LOW) {
    // Keep previous error to remember turn direction when line is lost
    if (previousError < 0) error = -3;
    else if (previousError > 0) error = 3;
  }

  // PID Calculations
  float P = error;
  integral += error;
  integral = constrain(integral, -100, 100); // Prevent integral windup
  float D = error - previousError;

  float correction = (Kp * P) + (Ki * integral) + (Kd * D);

  // Calculate motor speed differential based on PID correction
  int leftMotorSpeed  = BASE_SPEED + correction;
  int rightMotorSpeed = BASE_SPEED - correction;

  driveMotors(leftMotorSpeed, rightMotorSpeed);

  previousError = error;
  delay(10); // Loop timing stability
}