#define AIN1 4// left
#define AIN2 5
#define BIN1 8
#define BIN2 7
#define PWMA 3
#define PWMB 9
#define STBY 6

#define NUM_SENSORS 5
#define LINE_THRESHOLD 500  
#define ROTATIONS 6

float Kp = 0.94;//0.94;//0.89;
float Kd = 1.08;//1.06;//1.02;
float Ki = 0.0;
long integral = 0;
int lastError = 0;

char dir;

int basespeed_l = 160-30;
int basespeed_r = 160-30;

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


  pinMode(12, INPUT_PULLUP);
}

void loop() { 
  while (digitalRead(12)) {}
  delay(3000);
  calibrate3();
  
  while (digitalRead(12)) {}
  delay(3000);
   while (1) {
     maze();
   }

}


void rotateCW() {
  rotate(150, 170);
}

void rotateCCW() {
  rotate_a(150, 170);
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





void linefollowPID() {
  while (1) {
    for (int i = 0; i < 5; i++) sensor[i] = analogRead(i);

    long weighted = 0;

    weighted += (sensor[1] - threshold[1]) * -1;
    weighted += (sensor[2] - threshold[2]) *  0;
    weighted += (sensor[3] - threshold[3]) *  1;

    float error = weighted / 2.0;

    float P = error;
    integral += error;
    integral = constrain(integral, -50, 50);

    float D = error - lastError;
    lastError = error;

    float correction = (Kp * P) + (Ki * integral) + (Kd * D);

    int leftPWM  = basespeed_l + correction;
    int rightPWM = basespeed_r - correction;

    leftPWM  = constrain(leftPWM, 0, maxPWM);
    rightPWM = constrain(rightPWM, 0, maxPWM+20);

    analogWrite(PWMA, leftPWM);
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);

    analogWrite(PWMB, rightPWM);
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);

    if (sensor[0] > threshold[0] || sensor[4] > threshold[4]) {
      integral = 0;
      lastError = 0;
      return;
    }

    if (sensor[1] < threshold[1] &&
        sensor[2] < threshold[2] &&
        sensor[3] < threshold[3]) {
      integral = 0;
      lastError = 0;
      return;
    }
  }
}


void maze() {
  while (1) {
    linefollowPID(); 
     straight();
    delay(70); 
    stop();   

    int left_count = 0;
    int right_count = 0;
    for(int i=0; i<5; i++) {
       if(analogRead(0) > threshold[0]) left_count++;
       if(analogRead(4) > threshold[4]) right_count++;
    }

    unsigned char found_left = (left_count > 2);
    unsigned char found_right = (right_count > 2);
    unsigned char found_straight = 0;

    for (int i = 0; i < 5; i++) sensor[i] = analogRead(i);
    if (sensor[1] > threshold[1] || sensor[2] > threshold[2] || sensor[3] > threshold[3]) {
      found_straight = 1;
    }

//    dir = select_turnR(found_right, found_straight, found_left);
    dir = select_turnL(found_left, found_straight, found_right);
    turn(dir);
  }
}


char select_turnL(char found_left, char found_straight, char found_right){
  if (found_left)
    return 'L';
  else if (found_straight)
    return 'S';
  else if (found_right)
    return 'R';
  else
    return 'B';
}


char select_turnR(char found_right, char found_straight, char found_left)
{
  if (found_right)
    return 'R';
  else if (found_straight)
    return 'S';
  else if (found_left)
    return 'L';
  else
    return 'B';
}


void turn(char dir){
  int line_position;
  // According to dir perform a turn

  switch (dir)
  {
    case'L':
      left();
      break;
    case'R':
      right();
      break;

    case'B':
      uturn();
      break;
  }

}

void backward(){
  analogWrite(PWMA, basespeed_r);
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);

  analogWrite(PWMB, basespeed_l);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);  
}




// ----
void left() {

  analogWrite(PWMA, turnspeed_l);
  digitalWrite(AIN1,LOW);
  digitalWrite(AIN2,HIGH);
  
  analogWrite(PWMB,turnspeed_r);
  digitalWrite(BIN1,HIGH);
  digitalWrite(BIN2,LOW);


  delay(200);

  while (analogRead(2) < threshold[2]) {
  analogWrite(PWMA, turnspeed_l);
  digitalWrite(AIN1,LOW);
  digitalWrite(AIN2,HIGH);
  
  analogWrite(PWMB,turnspeed_r);
  digitalWrite(BIN1,HIGH);
  digitalWrite(BIN2,LOW);

  }
  analogWrite(PWMA,0);
  analogWrite(PWMB,0);
  digitalWrite(AIN1,LOW);
  digitalWrite(AIN2,LOW);
  digitalWrite(BIN1,LOW);
  digitalWrite(BIN2,LOW);
  delay(50);
}

void right() {
  analogWrite(PWMA,turnspeed_l);
  digitalWrite(AIN1,HIGH);
  digitalWrite(AIN2,LOW);

  analogWrite(PWMB,turnspeed_r);
  digitalWrite(BIN1,LOW);
  digitalWrite(BIN2,HIGH);
  
  delay(200);

  while (analogRead(2) < threshold[2]) {
  analogWrite(PWMA,turnspeed_l);
  digitalWrite(AIN1,HIGH);
  digitalWrite(AIN2,LOW);

  analogWrite(PWMB,turnspeed_l);
  digitalWrite(BIN1,LOW);
  digitalWrite(BIN2,HIGH);
  }
  analogWrite(PWMA,0);
  analogWrite(PWMB,0);
  digitalWrite(AIN1,LOW);
  digitalWrite(AIN2,LOW);
  digitalWrite(BIN1,LOW);
  digitalWrite(BIN2,LOW);
  delay(50);
}

void straight() {
  analogWrite(PWMA,basespeed_r);
  digitalWrite(AIN1,HIGH);
  digitalWrite(AIN2,LOW);  
  
  analogWrite(PWMB,basespeed_l);
  digitalWrite(BIN1,HIGH);
  digitalWrite(BIN2,LOW);
}

void inchforward() {
  analogWrite(PWMA,turnspeed_r);
  analogWrite(PWMB,turnspeed_l);
  digitalWrite(AIN1,HIGH);
  digitalWrite(AIN2,LOW);
  digitalWrite(BIN1,HIGH);
  digitalWrite(BIN2,LOW);
  delay(50);
}
void stop() {
  analogWrite(PWMA,0);
  analogWrite(PWMB,0);
  digitalWrite(AIN1,LOW);
  digitalWrite(AIN2,LOW);
  digitalWrite(BIN1,LOW);
  digitalWrite(BIN2,LOW);
}

void uturn() {
  analogWrite(PWMA,turnspeed_l);
  digitalWrite(AIN1,HIGH);
  digitalWrite(AIN2,LOW);
  
  analogWrite(PWMB,turnspeed_r);
  digitalWrite(BIN1,LOW);
  digitalWrite(BIN2,HIGH);


  delay(250);
  while (analogRead(2) < threshold[2]) {
  analogWrite(PWMA,turnspeed_l);
  digitalWrite(AIN1,HIGH);
  digitalWrite(AIN2,LOW);
  
  analogWrite(PWMB,turnspeed_r);
  digitalWrite(BIN1,LOW);
  digitalWrite(BIN2,HIGH);
  }
  analogWrite(PWMA,0);
  digitalWrite(AIN1,LOW);
  digitalWrite(AIN2,LOW);

  analogWrite(PWMB,0);
  digitalWrite(BIN1,LOW);
  digitalWrite(BIN2,LOW);
  delay(50);
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



void led(){
  digitalWrite(13, HIGH);
  delay(1000); 
  digitalWrite(13, LOW);
  delay(1000); 
}
