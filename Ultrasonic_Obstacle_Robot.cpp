// --- Ultrasonic Sensor Pins ---
#define TRIG_PIN 5  // Configured as Digital OUTPUT
#define ECHO_PIN 18 // Configured as Digital INPUT

// --- Motor Pins (L293D / L298N) ---
#define ENA 14 // Left Motor Speed (PWM)
#define IN1 27 // Left Motor Dir 1
#define IN2 26 // Left Motor Dir 2

#define ENB 32 // Right Motor Speed (PWM)
#define IN3 25 // Right Motor Dir 1
#define IN4 33 // Right Motor Dir 2

// --- Obstacle Threshold ---
const int OBSTACLE_DISTANCE = 20; // Distance threshold in centimeters
const int BASE_SPEED = 180;        // Motor speed (0-255)

// Function Declarations
float readDistanceCm();
void driveRobot(int leftSpeed, int rightSpeed);

void setup() {
  Serial.begin(115200);

  // 1. Ultrasonic Sensor Setup
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // 2. Motor Pins Setup
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // 3. Hardware PWM Setup (ESP32 Core v3.x)
  ledcAttach(ENA, 5000, 8);
  ledcAttach(ENB, 5000, 8);

  delay(1000); // Startup delay
}

void loop() {
  float distance = readDistanceCm();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Check if an obstacle is within the threshold distance
  if (distance > 0 && distance <= OBSTACLE_DISTANCE) {
    // Obstacle detected! Execute avoidance maneuver:
    
    // Step 1: Stop immediately
    driveRobot(0, 0);
    delay(200);

    // Step 2: Reverse slightly
    driveRobot(-BASE_SPEED, -BASE_SPEED);
    delay(300);

    // Step 3: Pivot Right to clear obstacle
    driveRobot(BASE_SPEED, -BASE_SPEED);
    delay(400);

    // Step 4: Pause briefly before re-checking forward path
    driveRobot(0, 0);
    delay(200);
  } else {
    // Path is clear: Drive Forward
    driveRobot(BASE_SPEED, BASE_SPEED);
  }

  delay(60); // Small stability delay between sensor readings
}

// --- Reads distance from HC-SR04 Ultrasonic Sensor ---
float readDistanceCm() {
  // Clear the Trigger pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Pulse HIGH for 10 microseconds to trigger ultrasonic pulse
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read Echo travel time in microseconds (with a 30ms timeout)
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  // Return calculated distance in cm (Speed of sound = 0.0343 cm/us)
  if (duration == 0) return 400.0; // Return safe default if out of range
  return (duration * 0.0343) / 2.0;
}

// --- Motor Control Helper Function ---
void driveRobot(int leftSpeed, int rightSpeed) {
  // Left Motor Direction
  if (leftSpeed >= 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    leftSpeed = -leftSpeed;
  }

  // Right Motor Direction
  if (rightSpeed >= 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    rightSpeed = -rightSpeed;
  }

  // Apply PWM Speed
  ledcWrite(ENA, leftSpeed);
  ledcWrite(ENB, rightSpeed);
}