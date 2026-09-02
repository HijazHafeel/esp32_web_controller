#include <WiFi.h>
#include <WebSocketsServer.h>

// --- Wi-Fi Credentials ---
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// --- Pin Definitions (Adjust pins to match your hardware) ---
#define ENA 14 // Left Motor Speed (PWM)
#define IN1 27 // Left Motor Dir 1
#define IN2 26 // Left Motor Dir 2

#define ENB 32 // Right Motor Speed (PWM)
#define IN3 25 // Right Motor Dir 1
#define IN4 33 // Right Motor Dir 2

// Initialize WebSocket server on port 81
WebSocketsServer webSocket = WebSocketsServer(81);

// Speed global variable
int currentSpeed = 200;

// --- Motor Driver Function ---
void driverRobot(int leftSpeed, int rightSpeed) {
  // Left Motor Direction & Speed
  if (leftSpeed >= 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    leftSpeed = -leftSpeed; // Convert to positive PWM value
  }

  // Right Motor Direction & Speed
  if (rightSpeed >= 0) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    rightSpeed = -rightSpeed; // Convert to positive PWM value
  }

  // Apply PWM signal using Zero Jitter Hardware PWM (ESP32 core v3.x style)
  ledcWrite(ENA, leftSpeed);
  ledcWrite(ENB, rightSpeed);
}

// --- WebSocket Event Handler ---
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    String msg = String((char*)payload);

    // Speed Slider Handling
    if (msg.startsWith("PWM:")) {
      currentSpeed = msg.substring(4).toInt();
    } 
    // Direction Commands
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

  // Configure Direction Pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Attach Hardware PWM Channels (5 kHz frequency, 8-bit resolution)
  ledcAttach(ENA, 5000, 8);
  ledcAttach(ENB, 5000, 8);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected.");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // Start WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  // Listen for incoming WebSocket events continuously
  webSocket.loop();
}