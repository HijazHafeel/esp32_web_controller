#define LED_PIN 2       // Built-in LED on many ESP32 boards

void setup() {
  ledcAttach(LED_PIN, 5000, 8);  // 5 kHz frequency, 8-bit resolution
}

void loop() {

  // Increase brightness
  for (int brightness = 0; brightness <= 255; brightness++) {
    ledcWrite(LED_PIN, brightness);
    delay(10);
  }

  // Decrease brightness
  for (int brightness = 255; brightness >= 0; brightness--) {
    ledcWrite(LED_PIN, brightness);
    delay(10);
  }
}