#define dirPin 2
#define stepPin 3
#define limitMin 10
#define limitMax 9

enum RunState {
  IDLE,
  MOVE_TO_MIN,
  MOVE_TO_MAX
};

RunState runState = IDLE;
unsigned int stepDelay = 500; // microseconds
unsigned long lastPrint = 0;

// ===== LỌC NHIỄU THEO THỜI GIAN =====
bool limitPressedTimeStable(int pin) {
  static unsigned long tMinLow = 0;
  static unsigned long tMaxLow = 0;

  unsigned long *tLow = (pin == limitMin) ? &tMinLow : &tMaxLow;

  if (digitalRead(pin) == LOW) {
    if (*tLow == 0) *tLow = millis();
    if (millis() - *tLow >= 1000) return true; // >=20ms
  } else {
    *tLow = 0;
  }
  return false;
}

void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(limitMin, INPUT_PULLUP);
  pinMode(limitMax, INPUT_PULLUP);

  Serial.begin(115200);

  digitalWrite(stepPin, LOW);
  digitalWrite(dirPin, HIGH);

  Serial.println("READY (TIME FILTER 20ms)");
}

void loop() {

  // ===== UART =====
  if (Serial.available()) {
    char cmd = Serial.read();

    if (cmd >= '1' && cmd <= '4') {
      runState = MOVE_TO_MIN;
      digitalWrite(dirPin, LOW);
      delayMicroseconds(10);
      Serial.println("MOVE TO MIN");
    }
    else if (cmd >= '5' && cmd <= '8') {
      runState = MOVE_TO_MAX;
      digitalWrite(dirPin, HIGH);
      delayMicroseconds(10);
      Serial.println("MOVE TO MAX");
    }
  }

  // ===== DEBUG (mỗi 400ms) =====
  if (millis() - lastPrint >= 400) {
    lastPrint = millis();

    Serial.print("RAW Min=");
    Serial.print(digitalRead(limitMin));
    Serial.print(" Max=");
    Serial.print(digitalRead(limitMax));

    Serial.print(" | TIME Min=");
    Serial.print(limitPressedTimeStable(limitMin));
    Serial.print(" Max=");
    Serial.print(limitPressedTimeStable(limitMax));

    Serial.print(" | STATE=");
    Serial.println(runState);
  }

  // ===== RUN =====
  switch (runState) {
    case MOVE_TO_MIN:
      if (!limitPressedTimeStable(limitMin)) {
        stepOnce();
      } else {
        runState = IDLE;
        Serial.println("REACHED MIN - STOP");
      }
      break;

    case MOVE_TO_MAX:
      if (!limitPressedTimeStable(limitMax)) {
        stepOnce();
      } else {
        runState = IDLE;
        Serial.println("REACHED MAX - STOP");
      }
      break;

    case IDLE:
    default:
      break;
  }
}

// ===== STEP =====
void stepOnce() {
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(stepDelay);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(stepDelay);
}