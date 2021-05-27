#define UP_DOWN_PIN A1
#define LEFT_RIGHT_PIN A0

void setup() {
  // put your setup code here, to run once:
  pinMode(UP_DOWN_PIN, INPUT);
  pinMode(LEFT_RIGHT_PIN, INPUT);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  int in_y = 2048 - analogRead(UP_DOWN_PIN);
  int in_x = 2048 - analogRead(LEFT_RIGHT_PIN);

  if (-150 < in_y && in_y < 150) {
    in_y = 0;
  }

  if (-150 < in_x && in_x < 150) {
    in_x = 0;
  }


  Serial.print(in_x);
  Serial.print(',');
  Serial.println(in_y);
}
