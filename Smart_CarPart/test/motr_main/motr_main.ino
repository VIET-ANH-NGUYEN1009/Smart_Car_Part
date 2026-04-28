#define dirPin 5
#define stepPin 6

const int stepsPerRev = 200;
const int steps90 = stepsPerRev / 4; // 50 steps

void stepMotor(int steps) {
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
  }
}

void setup() {
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);

  digitalWrite(dirPin, HIGH); // CW
}

void loop() {

  // 0° -> 90°
  stepMotor(steps90);
  delay(1000);

  // 90° -> 180°
  stepMotor(steps90);
  delay(1000);

  // 180° -> 270°
  stepMotor(steps90);
  delay(1000);

  // DỪNG TẠI 270°
  while (1);
}