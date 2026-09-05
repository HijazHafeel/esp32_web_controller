const int IR_ANALOG_PIN = A0; 
const int THRESHOLD = 500; // Adjust this value based on ambient room light

void setup() {
  Serial.begin(9600);
}

void loop() {
  int analogVal = analogRead(IR_ANALOG_PIN);

  Serial.print("IR Reading: ");
  Serial.print(analogVal);

  if (analogVal < THRESHOLD) {
    Serial.println(" -> LIGHT Surface");
  } else {
    Serial.println(" -> DARK Surface");
  }

  delay(200);
}