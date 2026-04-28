#include <Arduino.h>
#include <Servo.h>

// =================================================
// CẤU HÌNH PIN
// =================================================
#define dirPin1   2    
#define stepPin1  3
#define limitMin 10    
#define limitMax  9    
#define dirPin2     5  
#define stepPin2    6
#define enaPin2     8  
#define limitMotor 12 
#define SERVO_PIN  A0
#define LIMIT_SERVO 11 

Servo myServo;

// =================================================
// THÔNG SỐ HIỆU CHỈNH
// =================================================
const float STEPS_PER_REV = 1600.0; 
const float STEP_PER_DEG = STEPS_PER_REV / 360.0;
int slotAngle[] = { 10, 90, 190, 280 }; 

#define LIFT_3CM_TIME 5000    
#define BACK_5CM_TIME 450     
#define SPEED_FWD 180         // Chiều ĐẨY RA
#define SPEED_REV 0           // Chiều THU VỀ
#define SPEED_STOP 90

long currentStep2 = 0;   
unsigned int interval1 = 500; 
unsigned int interval2 = 1600; 
unsigned long tServo = 0;

// =================================================
// HÀM HỖ TRỢ
// =================================================
bool isLimit(int pin, unsigned long &tLow) {
  if (digitalRead(pin) == LOW) {
    if (tLow == 0) tLow = millis();
    if (millis() - tLow >= 50) return true;
  } else { tLow = 0; }
  return false;
}

void moveM1(int dir, int targetPin) {
  digitalWrite(dirPin1, dir);
  unsigned long t = 0;
  while (!isLimit(targetPin, t)) {
    digitalWrite(stepPin1, HIGH); delayMicroseconds(interval1);
    digitalWrite(stepPin1, LOW);  delayMicroseconds(interval1);
  }
}

void action3cm(int dir) {
  digitalWrite(dirPin1, dir);
  unsigned long start = millis();
  while (millis() - start < LIFT_3CM_TIME) {
    digitalWrite(stepPin1, HIGH); delayMicroseconds(interval1);
    digitalWrite(stepPin1, LOW);  delayMicroseconds(interval1);
  }
}

void moveM2(float angle) {
  long targetStep = round(angle * STEP_PER_DEG);
  digitalWrite(dirPin2, (currentStep2 < targetStep) ? HIGH : LOW);
  while (currentStep2 != targetStep) {
    digitalWrite(stepPin2, HIGH); delayMicroseconds(interval2);
    digitalWrite(stepPin2, LOW);  delayMicroseconds(interval2);
    currentStep2 += (currentStep2 < targetStep) ? 1 : -1;
  }
}

// =================================================
// QUY TRÌNH GỬI XE
// =================================================
void guiXe(int slot) {
  Serial.println("--- START GUI XE ---");
  moveM2(0); moveM1(LOW, limitMin);
  delay(3000); 

  if (slot <= 4) { // TẦNG 1
    myServo.write(SPEED_FWD); tServo = 0; while (!isLimit(LIMIT_SERVO, tServo));
    myServo.write(SPEED_STOP); delay(500);
    action3cm(HIGH);
    myServo.write(SPEED_REV); delay(BACK_5CM_TIME); myServo.write(SPEED_STOP);
    moveM2(slotAngle[(slot-1)%4]);
    myServo.write(SPEED_FWD); tServo = 0; while (!isLimit(LIMIT_SERVO, tServo));
    myServo.write(SPEED_STOP); delay(500);
    moveM1(LOW, limitMin);
    myServo.write(SPEED_REV); delay(BACK_5CM_TIME); myServo.write(SPEED_STOP);
  } 
  else { // TẦNG 2
    myServo.write(SPEED_FWD); tServo = 0; while (!isLimit(LIMIT_SERVO, tServo));
    myServo.write(SPEED_STOP); delay(500);
    action3cm(HIGH);
    myServo.write(SPEED_REV); delay(BACK_5CM_TIME); myServo.write(SPEED_STOP);
    moveM1(HIGH, limitMax);
    moveM2(slotAngle[(slot-1)%4]);
    myServo.write(SPEED_FWD); tServo = 0; while (!isLimit(LIMIT_SERVO, tServo));
    myServo.write(SPEED_STOP); delay(500);
    action3cm(LOW);
    myServo.write(SPEED_REV); delay(BACK_5CM_TIME); myServo.write(SPEED_STOP);
    moveM1(LOW, limitMin);
  }
  Serial.println("GUI XE DONE!");
}

// =================================================
// QUY TRÌNH LẤY XE (ĐÃ SỬA TẦNG 1 THEO YÊU CẦU)
// =================================================
void layXe(int slot) {
  Serial.println("--- START LAY XE ---");
  
  if (slot <= 4) { 
    // TẦNG 1: Xoay đến ô đỗ -> Đẩy cần -> Nâng lên 3cm -> Về cổng 0 -> Đẩy cần ra -> Hạ về LimitMin -> Thu cần về
    moveM2(slotAngle[(slot-1)%4]);
    myServo.write(SPEED_FWD); tServo = 0; while (!isLimit(LIMIT_SERVO, tServo));
    myServo.write(SPEED_STOP); delay(500);
    action3cm(HIGH);
    moveM2(0);
    myServo.write(SPEED_FWD); tServo = 0; while (!isLimit(LIMIT_SERVO, tServo));
    myServo.write(SPEED_STOP); delay(500);
    moveM1(LOW, limitMin); // Hạ về sàn tuyệt đối (LimitMin)
    myServo.write(SPEED_REV); delay(BACK_5CM_TIME); myServo.write(SPEED_STOP);
  } 
  else { 
    // TẦNG 2: Xoay -> Lên Max -> Hạ 3cm -> Đẩy cần -> Lên Max -> Thu cần -> Về 0 -> Hạ sàn -> Nhấc 3cm -> Đẩy cần -> Hạ sàn -> Thu cần
    moveM2(slotAngle[(slot-1)%4]);
    moveM1(HIGH, limitMax);
    action3cm(LOW);
    myServo.write(SPEED_FWD); tServo = 0; while (!isLimit(LIMIT_SERVO, tServo));
    myServo.write(SPEED_STOP); delay(500);
    moveM1(HIGH, limitMax);
    myServo.write(SPEED_REV); delay(BACK_5CM_TIME); myServo.write(SPEED_STOP);
    moveM2(0);
    moveM1(LOW, limitMin);
    action3cm(HIGH);
    myServo.write(SPEED_FWD); tServo = 0; while (!isLimit(LIMIT_SERVO, tServo));
    myServo.write(SPEED_STOP); delay(500);
    moveM1(LOW, limitMin);
    myServo.write(SPEED_REV); delay(BACK_5CM_TIME); myServo.write(SPEED_STOP);
  }
  Serial.println("LAY XE DONE!");
}

void setup() {
  pinMode(stepPin1, OUTPUT); pinMode(dirPin1, OUTPUT);
  pinMode(limitMin, INPUT_PULLUP); pinMode(limitMax, INPUT_PULLUP);
  pinMode(stepPin2, OUTPUT); pinMode(dirPin2, OUTPUT);
  pinMode(enaPin2, OUTPUT);  pinMode(limitMotor, INPUT_PULLUP);
  pinMode(LIMIT_SERVO, INPUT_PULLUP);
  myServo.attach(SERVO_PIN); myServo.write(SPEED_STOP);
  digitalWrite(enaPin2, LOW); 
  Serial.begin(115200);
  moveM1(LOW, limitMin);
  digitalWrite(dirPin2, LOW);
  unsigned long th = 0;
  while (!isLimit(limitMotor, th)) {
    digitalWrite(stepPin2, HIGH); delayMicroseconds(800);
    digitalWrite(stepPin2, LOW);  delayMicroseconds(800);
  }
  currentStep2 = 0;
  Serial.println("SYSTEM READY!");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n'); cmd.trim();
    if (cmd.startsWith("r")) layXe(cmd.substring(1).toInt());
    else if (cmd.length() > 0) guiXe(cmd.toInt());
  }
}
