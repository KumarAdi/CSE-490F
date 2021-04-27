void setup() {
  // put your setup code here, to run once:
  pinMode(3, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  const int brightness = map(analogRead(A0), 1023, 0, 0, 255);
  analogWrite(3,  brightness);
}
