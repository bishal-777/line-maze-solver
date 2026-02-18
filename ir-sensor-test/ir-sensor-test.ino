

void setup() {
  Serial.begin(9600);
  pinMode(13, HIGH);
}

void loop() {
  int s0 = analogRead(A0);
  int s1 = analogRead(A1);
  int s2 = analogRead(A2);
  int s3 = analogRead(A3);
  int s4 = analogRead(A4);

  Serial.print("S0: ");
  Serial.print(s0);
  Serial.print("\t");
  Serial.print(" | S1: ");
  Serial.print(s1);
  Serial.print("\t");
  Serial.print(" | S2: ");
  Serial.print(s2);
  Serial.print("\t");
  Serial.print(" | S3: ");
  Serial.print(s3);
  Serial.print("\t");
  Serial.print(" | S4: ");
  Serial.println(s4);
  Serial.print("\t");

  delay(500);
}
