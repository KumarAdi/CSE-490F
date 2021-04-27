void setup() {
  pinMode(13, INPUT);
  pinMode(12, INPUT);
  pinMode(11, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(3, digitalRead(13));
  digitalWrite(4, !digitalRead(12));
  digitalWrite(5, !digitalRead(11));
}
