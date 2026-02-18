//#define AIN1 5// left
//#define AIN2 4
//#define BIN1 7
//#define BIN2 8
//#define PWMA 3
//#define PWMB 9
//#define STBY 6

#define AIN1 4// left
#define AIN2 5
#define BIN1 8
#define BIN2 7
#define PWMA 3
#define PWMB 9
#define STBY 6

void setup() {
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(STBY, OUTPUT);

  digitalWrite(STBY, HIGH);  // enable motor driver
}

void loop() {
//  digitalWrite(AIN1, HIGH);
//  digitalWrite(AIN2, LOW);
//  analogWrite(PWMA, 150);   
//  delay(2000);
//
//  analogWrite(PWMA, 0);     
//  delay(1000);
//
//  digitalWrite(BIN1, HIGH);
//  digitalWrite(BIN2, LOW);
//  analogWrite(PWMB, 150);
//  delay(2000);
//
//  analogWrite(PWMB, 0);     
//  delay(1000);

  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);

  analogWrite(PWMA, 160);
  analogWrite(PWMB, 150);
  delay(1000);


//  analogWrite(PWMA, 0);
//  analogWrite(PWMB, 0);
//  delay(1000);
//
//  digitalWrite(AIN1, LOW);
//  digitalWrite(AIN2, HIGH);
//  digitalWrite(BIN1, LOW);
//  digitalWrite(BIN2, HIGH);
//
//  analogWrite(PWMA, 150);
//  analogWrite(PWMB, 10);
//  delay(1000);


  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
  delay(1000);  
////
//  digitalWrite(AIN1, HIGH);
//  digitalWrite(AIN2, LOW);
//  digitalWrite(BIN1, LOW);
//  digitalWrite(BIN2, HIGH);
//
//  analogWrite(PWMA, 90);
//  analogWrite(PWMB, 90);
//  delay(500);
//
//  analogWrite(PWMA, 0);
//  analogWrite(PWMB, 0);
//  delay(1000);
}
