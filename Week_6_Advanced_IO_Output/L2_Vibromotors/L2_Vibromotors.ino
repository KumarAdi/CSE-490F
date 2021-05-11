#define VIBRO_PIN 6

void setup() {
  // put your setup code here, to run once:
  pinMode(VIBRO_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(VIBRO_PIN, HIGH);
  delay(1000);
  digitalWrite(VIBRO_PIN, LOW);
  delay(1000);
}
