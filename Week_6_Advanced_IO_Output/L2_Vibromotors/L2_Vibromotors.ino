#define VIBRO_PIN 6
#define POT_PIN A1

void setup() {
  // put your setup code here, to run once:
  pinMode(VIBRO_PIN, OUTPUT);
  pinMode(POT_PIN, INPUT);
}

float idleTime = 1.0;

void loop() {
  // put your main code here, to run repeatedly:
  // Part 2 code:
//  const int pot_in = analogRead(POT_PIN);
//  const int vibro_out = map(pot_in, 0, 1024, 0, 255);
//  analogWrite(VIBRO_PIN, vibro_out);
//  delay(250);

  // Part 3 code:
  digitalWrite(VIBRO_PIN, HIGH);
  delay((1 - idleTime) * 1000);
  digitalWrite(VIBRO_PIN, LOW);
  delay(idleTime * 1000);

  idleTime /= 1.25;
}
