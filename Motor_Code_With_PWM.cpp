#define IN1 25
#define IN2 26
#define ENA 27

#define IN3 32
#define IN4 33
#define ENB 14

void setup() {

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // PWM
  ledcAttach(ENA, 5000, 8);
  ledcAttach(ENB, 5000, 8);
}

void loop() {

  // ------------- FORWARD -------------

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  // Speed = 200 / 255
  ledcWrite(ENA, 200);
  ledcWrite(ENB, 200);

  delay(3000);


  // ------------- STOP -------------

  ledcWrite(ENA, 0);
  ledcWrite(ENB, 0);

  delay(1000);


  // ------------- BACKWARD -------------

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  ledcWrite(ENA, 150);
  ledcWrite(ENB, 150);

  delay(3000);


  // ------------- STOP -------------

  ledcWrite(ENA, 0);
  ledcWrite(ENB, 0);

  delay(1000);
}