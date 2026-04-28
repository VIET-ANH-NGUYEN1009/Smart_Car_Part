#define dirPin 2
#define stepPin 3

#define limitMin 10
#define limitMax 9

bool direction = true; // true = HIGH, false = LOW

void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  pinMode(limitMin, INPUT_PULLUP);
  pinMode(limitMax, INPUT_PULLUP);

  digitalWrite(dirPin, direction ? HIGH : LOW);
}

void loop() {
  int minState = digitalRead(limitMin);
  int maxState = digitalRead(limitMax);

  // ===== XỬ LÝ LIMIT =====
  // Nếu đang quay CW (HIGH) và chạm limitMax → đổi chiều
  if (direction == true && maxState == HIGH ) {
    direction = false;
    digitalWrite(dirPin, LOW);
    delay(50); // chống dội + bảo vệ cơ khí
  }

  // Nếu đang quay CCW (LOW) và chạm limitMin → đổi chiều
  if (direction == false && minState == HIGH) {
    direction = true;
    digitalWrite(dirPin, HIGH);
    delay(50);
  }
  // ======================

  // Step motor
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(500);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(500);
}