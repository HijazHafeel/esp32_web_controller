#include <WiFi.h>
#include <WebSocketsServer.h>

// --- Wi-Fi Credentials ---
const char* ssid = "HUS";
const char* password = "hus@hmg71530";

// --- Motor Pin Definitions ---
#define ENA 14 // Left Motor Speed (PWM)
#define IN1 27 // Left Motor Dir 1
#define IN2 26 // Left Motor Dir 2

#define ENB 32 // Right Motor Speed (PWM)
#define IN3 25 // Right Motor Dir 1
#define IN4 33 // Right Motor Dir 2

// --- PWM Settings (ESP32 Core v2.x) ---
#define PWM_FREQ 5000
#define PWM_RES 8
#define LEFT_PWM_CHANNEL 0
#define RIGHT_PWM_CHANNEL 1

// --- Server & State Variables ---
WebSocketsServer webSocket = WebSocketsServer(81);
int currentSpeed = 200;

// --- Motor Control Function ---
void driverRobot(int leftSpeed, int rightSpeed) {
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

  // Constrain PWM values to 0-255 range
  leftSpeed = constrain(leftSpeed, 0, 255);
  rightSpeed = constrain(rightSpeed, 0, 255);

  // Apply PWM speeds
  ledcWrite(LEFT_PWM_CHANNEL, leftSpeed);
  ledcWrite(RIGHT_PWM_CHANNEL, rightSpeed);
}

// --- WebSocket Event Handler ---
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    String msg = String((char*)payload);

    // Speed Slider Command ("PWM:xxx")
    if (msg.startsWith("PWM:")) {
      currentSpeed = msg.substring(4).toInt();
      Serial.print("Speed updated to: ");
      Serial.println(currentSpeed);
    } 
    // Direction Commands ('F', 'B', 'L', 'R', 'S')
    else if (payload[0] == 'F') {
      driverRobot(currentSpeed, currentSpeed); // Forward
    } else if (payload[0] == 'B') {
      driverRobot(-currentSpeed, -currentSpeed); // Backward
    } else if (payload[0] == 'L') {
      driverRobot(100, currentSpeed); // Turn Left
    } else if (payload[0] == 'R') {
      driverRobot(currentSpeed, 100); // Turn Right
    } else if (payload[0] == 'S') {
      driverRobot(0, 0); // Stop
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Set motor direction pins as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Configure PWM channels
  ledcSetup(LEFT_PWM_CHANNEL, PWM_FREQ, PWM_RES);
  ledcSetup(RIGHT_PWM_CHANNEL, PWM_FREQ, PWM_RES);
  
  ledcAttachPin(ENA, LEFT_PWM_CHANNEL);
  ledcAttachPin(ENB, RIGHT_PWM_CHANNEL);

  // Stop motors initially
  driverRobot(0, 0);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected.");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // Start WebSocket Server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
}
