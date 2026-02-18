#define AIN1 4// left
#define AIN2 5
#define BIN1 8
#define BIN2 7
#define PWMA 3
#define PWMB 9
#define STBY 6

#define NUM_SENSORS 5
#define LINE_THRESHOLD 600  
#define ROTATIONS 3


float Kp = 0.25;//0.94;//0.92;//0.89;//0.89;//0.89;//0.86;
float Kd = 1;//1.06;//1.04;//1.02;//1.02;//1.01;//1.0;
float Ki = 0.0;
long integral = 0;
int lastError = 0;

char dir;

int basespeed_l = 160-60;
int basespeed_r = 160+20-60;

int turnspeed_l = 90;
int turnspeed_r = 90;


int maxPWM = 170;

int minVal[5], maxVal[5], threshold[5];
int sensor[5];

String str;




void setup() {
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH); // enable driver


  pinMode(2, INPUT_PULLUP);
}

void loop() { 
while (digitalRead(2)) {}
delay(3000);
calibrate();

while (digitalRead(2)) {}
delay(3000);

  while (1) {
    linefollowPID_v2();
  }

}


bool frontOnLine() {
  return (analogRead(1) > LINE_THRESHOLD ||
          analogRead(2) > LINE_THRESHOLD ||
          analogRead(3) > LINE_THRESHOLD);
}

bool backOnLine() {
  return (analogRead(0) > LINE_THRESHOLD ||
          analogRead(4) > LINE_THRESHOLD);
}


void rotate(int spA, int spB) {
  analogWrite(PWMA, spA);
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);

  analogWrite(PWMB, spB);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
}
void rotate_a(int spA, int spB) {
  analogWrite(PWMA, spA);
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);

  analogWrite(PWMB, spB);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
}

void rotateCW() {
  rotate(150, 170);
}

void rotateCCW() {
  rotate_a(150, 170);
}


void rotateExact(int rotations, bool clockwise) {

  int count = 0;
  bool frontSeen = false;
  bool backSeen  = false;

  while (frontOnLine()) {
    clockwise ? rotateCW() : rotateCCW();
    delay(5);
  }

  clockwise ? rotateCW() : rotateCCW();

  while (count < rotations) {

    for (int i = 0; i < 5; i++) {
      int v = analogRead(i);
      if (v < minVal[i]) minVal[i] = v;
      if (v > maxVal[i]) maxVal[i] = v;
    }

    if (!frontSeen && frontOnLine()) {
      frontSeen = true;
      delay(100);
    }

    if (frontSeen && !backSeen && backOnLine()) {
      backSeen = true;
      delay(100);
    }

    if (frontSeen && backSeen && frontOnLine()) {
      count++;
      frontSeen = false;
      backSeen  = false;
      delay(150);
    }

    delay(5);
  }

  stop();
  delay(500);
}


void calibrate3() {

  for (int i = 0; i < 5; i++) {
    int v = analogRead(i);
    minVal[i] = v;
    maxVal[i] = v;
  }

  rotateExact(ROTATIONS, true);

  delay(1000);

  rotateExact(ROTATIONS, false);

  for (int i = 0; i < 5; i++) {
    threshold[i] = (minVal[i] + maxVal[i]) / 2;
  }
}

void calibrate() {
  for (int i = 0; i < NUM_SENSORS; i++) {
    minVal[i] = 1023;
    maxVal[i] = 0;
  }

  unsigned long start = millis();
  while (millis() - start < 5000) {
    rotate(150, 150);
    for (int i = 0; i < NUM_SENSORS; i++) {
      int v = analogRead(i);
      minVal[i] = min(minVal[i], v);
      maxVal[i] = max(maxVal[i], v);
    }
  }

  stop();
  delay(500);

  for (int i = 0; i < NUM_SENSORS; i++) {
    threshold[i] = (minVal[i] + maxVal[i]) / 2;
  }
}


void linefollowPID_v1() {


  for (int i = 0; i < 5; i++) sensor[i] = analogRead(i);

  long weighted = 0;

  // weight sum for normalization: |-2| + |-1| + 0 + |1| + |2| = 6
  const float normFactor = 6.0;

  weighted += (sensor[0] - threshold[0]) * -2;
  weighted += (sensor[1] - threshold[1]) * -1;
  weighted += (sensor[2] - threshold[2]) *  0;
  weighted += (sensor[3] - threshold[3]) *  1;
  weighted += (sensor[4] - threshold[4]) *  2;

  float error = weighted / normFactor;


  float P = error;
  integral += error;
  integral = constrain(integral, -100, 100);  
  float D = error - lastError;
  lastError = error;

  float correction = (Kp * P) + (Ki * integral) + (Kd * D);

  int rightPWM = basespeed_r - correction;
  int leftPWM  = basespeed_l + correction;

  rightPWM = constrain(rightPWM, 0, maxPWM);
  leftPWM  = constrain(leftPWM, 0, maxPWM+20);

  analogWrite(PWMA, leftPWM);
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);

  analogWrite(PWMB, rightPWM);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);

}

void linefollowPID_v2() {
  while (1) {
    for (int i = 0; i < 5; i++) sensor[i] = analogRead(i);

    long weighted = 0;


    weighted += (sensor[1] - threshold[1]) * -1;
    weighted += (sensor[2] - threshold[2]) *  0;
    weighted += (sensor[3] - threshold[3]) *  1;

    float error = weighted / 2.0;

    float P = error;
    integral += error;
    integral = constrain(integral, -100, 100);

    float D = error - lastError;
    lastError = error;

    float correction = (Kp * P) + (Ki * integral) + (Kd * D);

    int leftPWM  = basespeed_l + correction;
    int rightPWM = basespeed_r - correction;

    leftPWM  = constrain(leftPWM, 0, maxPWM + 20);
    rightPWM = constrain(rightPWM, 0, maxPWM);

    analogWrite(PWMA, leftPWM);
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);

    analogWrite(PWMB, rightPWM);
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  }
}
void stop() {
  analogWrite(PWMA,0);
  analogWrite(PWMB,0);
  digitalWrite(AIN1,LOW);
  digitalWrite(AIN2,LOW);
  digitalWrite(BIN1,LOW);
  digitalWrite(BIN2,LOW);
}
