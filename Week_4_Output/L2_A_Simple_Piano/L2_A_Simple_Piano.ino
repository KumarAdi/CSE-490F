const int KEY_C = 262;  // 261.6256 Hz (middle C)
const int KEY_D = 294 ; // 293.6648 Hz
const int KEY_E = 330 ; // 329.6276 Hz
const int KEY_F = 350 ; // 349.2282 Hz
const int KEY_G = 392 ; // 391.9954 Hz
const int KEY_A = 440;
const int KEY_B = 494;
const int KEY_C2 = 524;

void setup() {
  pinMode(13, INPUT);
  pinMode(12, INPUT);
  pinMode(11, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  const bool c = digitalRead(13);
  const bool d = !digitalRead(12);
  const bool e = !digitalRead(11);
  const bool f = !digitalRead(10);
  const bool g = !digitalRead(9);
  const bool a = !digitalRead(8);
  const bool b = !digitalRead(7);
  const bool c2 = !digitalRead(6);

  int pitch = KEY_C;
  if (c) pitch = KEY_C;
  else if (d) pitch = KEY_D;
  else if (e) pitch = KEY_E;
  else if (f) pitch = KEY_F;
  else if (g)  pitch = KEY_G;
  else if (a) pitch = KEY_A;
  else if (b) pitch = KEY_B;
  else pitch = KEY_C2;

  if (c || d || e || f || g || a || b || c2) {
    tone(3, pitch);
  } else {
    noTone(3);
  }
}
