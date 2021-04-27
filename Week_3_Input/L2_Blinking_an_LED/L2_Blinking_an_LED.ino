const int LED_PORT = 3;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PORT, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_PORT, HIGH);
  Serial.println("Pin 3 is HIGH");
  delay(1000);
  digitalWrite(LED_PORT, LOW);
  Serial.println("Pin 3 is LOW");
  delay(1000);
}
