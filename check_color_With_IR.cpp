const int IR_ANALOG_PIN = 34; // Connected to the sensor Signal/OUT pin
int THRESHOLD = 2000;         // Midpoint threshold (0 to 4095 range)

void setup() {
  Serial.begin(115200);
  
  // Set ADC attenuation to measure up to 3.3V accurately
  analogSetAttenuation(ADC_11db); 
}

void loop() {
  int rawValue = analogRead(IR_ANALOG_PIN);

  Serial.print("Raw Value: ");
  Serial.print(rawValue);

  // Reflective surfaces return lower voltage / lower raw readings on standard IR phototransistors
  if (rawValue < THRESHOLD) {
    Serial.println(" | Result: LIGHT Surface Detected");
  } else {
    Serial.println(" | Result: DARK Surface Detected");
  }

  delay(200);
}
