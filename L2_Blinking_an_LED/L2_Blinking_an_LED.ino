void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Pin 13 is HIGH");
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Pin 13 is LOW");
  delay(1000);
}
