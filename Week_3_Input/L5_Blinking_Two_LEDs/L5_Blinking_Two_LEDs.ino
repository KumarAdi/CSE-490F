const int LED1_OUTPUT_PIN = 3;
const int LED2_OUTPUT_PIN = 4;
const int DELAY_MS = 1000;


void setup() {
  // put your setup code here, to run once:
  pinMode(LED1_OUTPUT_PIN, OUTPUT);
  pinMode(LED2_OUTPUT_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED1_OUTPUT_PIN, HIGH);
  digitalWrite(LED2_OUTPUT_PIN, HIGH);
  delay(DELAY_MS);
  
  digitalWrite(LED1_OUTPUT_PIN, LOW);
  digitalWrite(LED2_OUTPUT_PIN, LOW);
  delay(DELAY_MS);
}
