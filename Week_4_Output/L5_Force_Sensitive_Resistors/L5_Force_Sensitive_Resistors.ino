const int KEY_C = 262;  // 261.6256 Hz (middle C)
const int KEY_D = 294 ; // 293.6648 Hz
const int KEY_E = 330 ; // 329.6276 Hz
const int KEY_F = 350 ; // 349.2282 Hz
const int KEY_G = 392 ; // 391.9954 Hz

int notes[] = {KEY_C, KEY_D, KEY_E, KEY_F, KEY_G};

void setup() {
  // put your setup code here, to run once:
  pinMode(3, OUTPUT);
  pinMode(11, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  const int readValue = analogRead(A0);
  const int pitch = map(readValue, 0, 1000, 0, 4);
  if (readValue > 0) {
    tone(11, notes[pitch]);
  } else {
    noTone(11);
  }
}
