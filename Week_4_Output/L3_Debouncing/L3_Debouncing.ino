#include "src/JC_Button/JC_Button.h"

const int KEY_C = 262;  // 261.6256 Hz (middle C)
const int KEY_D = 294 ; // 293.6648 Hz
const int KEY_E = 330 ; // 329.6276 Hz
const int KEY_F = 350 ; // 349.2282 Hz
const int KEY_G = 392 ; // 391.9954 Hz
const int KEY_A = 440;
const int KEY_B = 494;
const int KEY_C2 = 524;

Button c_btn(13, 50, false, false);
Button d_btn(12, 50, false);
Button e_btn(11, 50);
Button f_btn(10, 50);
Button g_btn(9, 50);
Button a_btn(8, 50);
Button b_btn(7, 50);
Button c2_btn(6, 50);


void setup() {
  c_btn.begin();
  d_btn.begin();
  e_btn.begin();
  f_btn.begin();
  g_btn.begin();
  a_btn.begin();
  b_btn.begin();
  c2_btn.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  const bool c = c_btn.read();
  const bool d = d_btn.read();
  const bool e = e_btn.read();
  const bool f = f_btn.read();
  const bool g = g_btn.read();
  const bool a = a_btn.read();
  const bool b = b_btn.read();
  const bool c2 = c2_btn.read();

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
