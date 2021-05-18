#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Tone32.hpp>
#include "src/JC_Button/JC_Button.h"

#define UP_DOWN_PIN A1
#define LEFT_RIGHT_PIN A0
#define BUZZER_PIN 14
#define LEFT_BTN_PIN 32
#define RIGHT_BTN_PIN 15

#define LIGHT_PIN_1 27
#define LIGHT_PIN_2 18
#define LIGHT_PIN_3 5
#define LIGHT_PIN_4 4
#define LIGHT_PIN_5 12

Button left_btn(LEFT_BTN_PIN, 50);
Button right_btn(RIGHT_BTN_PIN, 50);

#define TONE_OUTPUT_PIN 19
#define TONE_PWM_CHANNEL 0

Tone32 _tone32(TONE_OUTPUT_PIN, TONE_PWM_CHANNEL);

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define FOCAL_LENGTH 64

#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define sgn(x) (((x) > 0) - ((x) < 0))

static const char MENU_TEXT[] = "PRESS ANY BUTTON TO\n START THE GAME";
static const char GAME_OVER_TEXT[] = "GAME OVER!\n\n PRESS ANY BUTTON TO\n TRY AGAIN";

typedef enum _GameState {
  MENU = 0,
  PLAY = 1,
  GAME_OVER = 2
} GameState;

typedef enum _Orientation {
  LEFT,
  RIGHT
} Orientation;

class Drawable {
  public:
  virtual void draw(Adafruit_SSD1306& _display) = 0;
};

class Point3: Drawable {
  public:
  Point3(float x, float y, float z) : x(x), y(y), z(z){}

  Point3() : x(0), y(0), z(0){};

  Point3 operator+ (const Point3& r) {
    Point3 ret = Point3(this->x + r.x, this->y + r.y, this->z + r.z);
    return ret;
  }

  Point3 operator- (const Point3& r) {
    Point3 ret = Point3(this->x - r.x, this->y - r.y, this->z - r.z);
    return ret;
  }

  Point3 operator* (float scale) {
    Point3 ret = Point3(scale * this->x, scale * this->y, scale * this->z);
    return ret;
  }

  Point3 operator/ (float scale) {
    Point3 ret = Point3(scale / this->x, scale / this->y, scale / this->z);
    return ret;
  }

  float dot(const Point3& r) {
    return this->x * r.x + this->y * r.y + this->z * r.z;
  }

  Point3 cross (const Point3& r) {
    return Point3(
      this->y * r.z - this->z * r.y,
      this->z * r.x - this->x * r.z,
      this->x * r.y - this->y * r.x);
  }

  Point3 rotate (Point3 axis, float angle) {
    
    float unchanged_factor = cos(angle);
    float cross_factor = sqrt(1.0f - unchanged_factor * unchanged_factor);
    float axis_factor = (1.0f - unchanged_factor) * axis.dot(*this);

    Point3 ret = *this * unchanged_factor + this->cross(axis) * cross_factor + axis * axis_factor;

    float target_sqr_magnitude = this->get_sqr_magnitude();
    float actual_sqr_magnitude = ret.get_sqr_magnitude();
    ret = ret * sqrt(target_sqr_magnitude / actual_sqr_magnitude);

    return ret;
  }

  void rotateAbout (Point3 axis, float angle, Point3 origin) {
    *this = origin + (*this - origin).rotate(axis, angle);
  }

  void screenInvariantSetZ (float z) {
    int screenX, screenY;

    this->toScreenCoords(&screenX, &screenY);
    this->x = (screenX - (SCREEN_WIDTH / 2)) * (z + FOCAL_LENGTH) / FOCAL_LENGTH;
    this->y = (screenY - (SCREEN_HEIGHT / 2)) * (z + FOCAL_LENGTH) / FOCAL_LENGTH;
    this->z = z;
  }
  
  void draw(Adafruit_SSD1306& _display) {
    int x ,y;
    this->toScreenCoords(&x, &y);

    _display.drawPixel(x, y, SSD1306_WHITE);
  }

  void toScreenCoords(int *x, int *y) {
    *x = ((this->x * FOCAL_LENGTH) / (this->z + FOCAL_LENGTH)) + (SCREEN_WIDTH / 2);
    *y = ((this->y * FOCAL_LENGTH) / (this->z + FOCAL_LENGTH)) + (SCREEN_HEIGHT / 2) ;
  }

  float get_sqr_magnitude () {
    return this->dot(*this);
  }

  void print() {
    Serial.print("(");
    Serial.print(x);
    Serial.print(", ");
    Serial.print(y);
    Serial.print(", ");
    Serial.print(z);
    Serial.print(")");
  }

  void println() {
    this->print();
    Serial.println();
  }
  
  float x, y, z;
};

Point3 rotationCenter(0, 0, 64);

class Line3: Drawable {

  public:
  Line3 (Point3 a, Point3 b) {
    this->a = a;
    this->b = b;
  }
  
  void draw(Adafruit_SSD1306& _display) {
    int x_a, y_a;
    a.toScreenCoords(&x_a, &y_a);
    int x_b, y_b;
    b.toScreenCoords(&x_b, &y_b);

    _display.drawLine(x_a, y_a, x_b, y_b, SSD1306_WHITE);
  }

  void rotateAbout(Point3 axis, float angle, Point3 origin) {
    this->a.rotateAbout(axis, angle, origin);
    this->b.rotateAbout(axis, angle, origin);
  }

  void screenInvariantSetZ(float z) {
    this->a.screenInvariantSetZ(z);
    this->b.screenInvariantSetZ(z);
  }

  void randomizeDepth(float minDepth, float maxDepth) {
    float range = maxDepth - minDepth;
    this->a.screenInvariantSetZ(minDepth + (random(100 * range) / 100));
    this->b.screenInvariantSetZ(minDepth + (random(100 * range) / 100));
  }

  void println() {
    Serial.print("Line:"); a.print(); b.println();  
  }
  
  private:
  Point3 a, b;
};

class Cube3: Drawable {
  public:
  Cube3(Point3 pos, Point3 down, Point3 right, Point3 forward): pos(pos), down(down), right(right), forward(forward) {}

   void draw(Adafruit_SSD1306& _display) {
    Point3 front_bottom_right = this->pos + this-> forward + this->down + this->right;
    Point3 front_bottom_left  = this->pos + this-> forward + this->down - this->right;
    Point3 front_top_right    = this->pos + this-> forward - this->down + this->right;
    Point3 front_top_left     = this->pos + this-> forward - this->down - this->right;
    Point3 back_bottom_right  = this->pos - this-> forward + this->down + this->right;
    Point3 back_bottom_left   = this->pos - this-> forward + this->down - this->right;
    Point3 back_top_right     = this->pos - this-> forward - this->down + this->right;
    Point3 back_top_left      = this->pos - this-> forward - this->down - this->right;

    Line3(front_bottom_right, front_bottom_left).draw(_display);
    Line3(front_bottom_right, front_top_right).draw(_display);
    Line3(front_bottom_right, back_bottom_right).draw(_display);

    Line3(back_top_left, back_top_right).draw(_display);
    Line3(back_top_left, back_bottom_left).draw(_display);
    Line3(back_top_left, front_top_left).draw(_display);

    Line3 (front_top_right, front_top_left).draw(_display);
    Line3 (front_top_left, front_bottom_left).draw(_display);
    Line3 (front_bottom_left, back_bottom_left).draw(_display);
    Line3 (back_bottom_left, back_bottom_right).draw(_display);
    Line3 (back_bottom_right, back_top_right).draw(_display);
    Line3 (back_top_right, front_top_right).draw(_display);
  }

  void rotate(Point3 axis, float angle) {
    this->down = this->down.rotate(axis, angle);
    this->right = this->right.rotate(axis, angle);
    this->forward = this->forward.rotate(axis, angle);
  }

  void rotateAbout(Point3 axis, float angle, Point3 origin) {
    this->pos.rotateAbout(axis, angle, origin);
  }

  void move(Point3 displacement) {
    this->pos = this->pos + displacement;
  }

  private:
  Point3 pos, down, right, forward;
};

class Arrow3: Drawable {
  public:
  Arrow3 (Point3 center, int width, Orientation dir): shaft(Line3(center, center)), headA(Line3(center, center)), headB(Line3(center, center)) {
    Point3 left = center - Point3(width /2, 0, 0);
    Point3 right = center + Point3(width /2, 0, 0);

    Point3 tip = (dir == LEFT) ? left : right;
    Point3 base = (dir == LEFT) ? right : left;

    this->shaft = Line3(base, tip);
    this->headA = Line3(tip, center + Point3(0, width / 2, 0));
    this->headB = Line3(tip, center - Point3(0, width / 2, 0));
    
    this->shaft.screenInvariantSetZ(rotationCenter.z);
    this->headA.screenInvariantSetZ(rotationCenter.z);
    this->headB.screenInvariantSetZ(rotationCenter.z);
    this->println();
  }
  
  void draw(Adafruit_SSD1306& _display) {
    this->shaft.draw(_display);
    this->headA.draw(_display);
    this->headB.draw(_display);
  }

  void rotateAbout(Point3 axis, float angle, Point3 origin) {
    this->shaft.rotateAbout(axis, angle, origin);
    this->headA.rotateAbout(axis, angle, origin);
    this->headB.rotateAbout(axis, angle, origin);
  }

  void randomizeDepth(float minDepth, float maxDepth) {
    this->shaft.randomizeDepth(minDepth, maxDepth);
    this->headA.randomizeDepth(minDepth, maxDepth);
    this->headB.randomizeDepth(minDepth, maxDepth);
  }

  void println() {
    Serial.println("Arrow:");
    Serial.print("shaft: "); shaft.println(); 
    Serial.print("headA: "); headA.println(); 
    Serial.print("headB: "); headB.println(); 
  }
  
  private:
  Line3 shaft, headA, headB;
};

//Cube3 test_cube_1(
//  Point3(0,0,96),
//  Point3(0,16,0),
//  Point3(16,0,0),
//  Point3(0,0,16)
//);
//
//Cube3 test_cube_2(
//  Point3(0,32,64),
//  Point3(0,16,0),
//  Point3(16,0,0),
//  Point3(0,0,16)
//);
//
//Cube3 test_cube_3(
//  Point3(32,0,64),
//  Point3(0,16,0),
//  Point3(16,0,0),
//  Point3(0,0,16)
//);

Orientation test_code[] = {LEFT, RIGHT, RIGHT, LEFT, LEFT}; 
Arrow3 test_arrows[] = {
  Arrow3(Point3(-32, 0, 0), 12, LEFT),
  Arrow3(Point3(-16, 0, 0), 12, RIGHT),
  Arrow3(Point3(0, 0, 0), 12, RIGHT),
  Arrow3(Point3(16, 0, 0), 12, LEFT),
  Arrow3(Point3(32, 0, 0), 12, LEFT)
};

int startMillis;
GameState gameState, prevState;
//Cube3 gameObjects[3] = {test_cube_1, test_cube_2, test_cube_3};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(UP_DOWN_PIN, INPUT);
  pinMode(LEFT_RIGHT_PIN, INPUT);
  pinMode(LEFT_BTN_PIN, INPUT_PULLUP);
  pinMode(RIGHT_BTN_PIN, INPUT_PULLUP);

  pinMode(LIGHT_PIN_1, OUTPUT);
  pinMode(LIGHT_PIN_2, OUTPUT);
  pinMode(LIGHT_PIN_3, OUTPUT);
  pinMode(LIGHT_PIN_4, OUTPUT);
  pinMode(LIGHT_PIN_5, OUTPUT);

  pinMode(BUZZER_PIN, OUTPUT);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!_display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  gameState = MENU;
  startMillis = millis();

  _display.setTextSize(1);
  _display.setTextColor(WHITE, BLACK);

  left_btn.begin();
  right_btn.begin();


}

long timeLeft = 60000 - 5000;
long prevSysTimeMillis;
int curr_guess;

void setupPlayState() {
  for (int i = 0; i < 5; i++) {
    test_code[i] = (random(255) > 127) ? LEFT : RIGHT;
    test_arrows[i] = Arrow3(Point3(16 * i - 32, 0, 0), 12, test_code[i]);
  }
  
  for (Arrow3 *curr = test_arrows; curr - test_arrows < 5; curr++) {
   curr->randomizeDepth(rotationCenter.z / 2, rotationCenter.z * 3 / 2);
  }

  int randRotY = random(-300, 300);
  int randRotX = random(-300, 300);

  for (Arrow3 *curr = test_arrows; curr - test_arrows < 5; curr++) {
    curr->rotateAbout(Point3(sgn(randRotY),0, 0), randRotY / 100.0f, rotationCenter);
    curr->rotateAbout(Point3(0,sgn(randRotX), 0), randRotX / 100.0f, rotationCenter);
  }

  curr_guess = 0;
  timeLeft += 5000;
  prevSysTimeMillis = millis();
}

const int lightPins[5] = {
  LIGHT_PIN_1,
  LIGHT_PIN_2,
  LIGHT_PIN_3,
  LIGHT_PIN_4,
  LIGHT_PIN_5,
};

void runPlayState(int in_x, int in_y, bool left_btn, bool right_btn) {
//  for(Cube3 *curr = gameObjects; curr - gameObjects < 3; curr++) {
//   curr->rotateAbout(Point3(sgn(in_y),0, 0), in_y / 20480.0f, rotationCenter);
//   curr->rotate(Point3(sgn(in_y),0, 0), in_y / 20480.0f);
//   curr->rotateAbout(Point3(0,sgn(in_x), 0), in_x / 20480.0f, rotationCenter);
//   curr->rotate(Point3(0,sgn(in_x), 0), in_x / 20480.0f);
//   curr->draw(_display);
//  }
  Orientation guessedOrientation = (left_btn) ? LEFT : RIGHT;

  if (left_btn || right_btn) {
    if (test_code[curr_guess] != guessedOrientation) {
      digitalWrite(BUZZER_PIN, HIGH);
      curr_guess = 0;
    } else {
      _tone32.playNote(NOTE_B, 4, 250);
      curr_guess++;

      if (curr_guess >= 5) {
        _tone32.playNote(NOTE_E, 5, 500);
      }
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  for (Arrow3 *curr = test_arrows; curr - test_arrows < 5; curr++) {
    curr->rotateAbout(Point3(sgn(in_y),0, 0), in_y / 20480.0f, rotationCenter);
    curr->rotateAbout(Point3(0,sgn(in_x), 0), in_x / 20480.0f, rotationCenter);
    curr->draw(_display);
  }

  timeLeft -= millis() - prevSysTimeMillis;
  prevSysTimeMillis = millis();

  if (timeLeft <= 0) {
    gameState = GAME_OVER;
  }

  _display.setCursor(115, 5);
  _display.print(timeLeft / 1000);

  for (int i = 0; i < curr_guess; i++) {
    digitalWrite(lightPins[i], HIGH);
  }
  for (int i = curr_guess; i < 5; i++) {
    digitalWrite(lightPins[i], LOW);
  }
  if (curr_guess >= 5) {
    setupPlayState();
  }
}

void loop() {
  int in_y = analogRead(UP_DOWN_PIN) - 2048;
  int in_x = 2048 - analogRead(LEFT_RIGHT_PIN);

  if (-150 < in_y && in_y < 150) {
    in_y = 0;
  }

  if (-150 < in_x && in_x < 150) {
    in_x = 0;
  }

  Point3(in_x, in_y, 0).println();

  left_btn.read();
  right_btn.read();
  
  // put your main code here, to run repeatedly:
  _display.clearDisplay();
  
  switch (gameState) {
    case MENU:
      int16_t x, y;
      uint16_t textWidth, textHeight;
      _display.getTextBounds(MENU_TEXT, 0, 0, &x, &y, &textWidth, &textHeight);
      _display.setCursor(_display.width() / 2 - textWidth / 2, _display.height() / 2 - textHeight / 2);
      _display.print(MENU_TEXT);

      if (left_btn.wasPressed() || right_btn.wasPressed()) {
        gameState = PLAY;
        setupPlayState();
        timeLeft = 60000 - 5000;
      }
      break;
    case PLAY:
      if (prevState != PLAY) {
        setupPlayState();
      }
      runPlayState(in_x, in_y, left_btn.wasPressed(), right_btn.wasPressed());
      break;
    case GAME_OVER:
      _display.getTextBounds(GAME_OVER_TEXT, 0, 0, &x, &y, &textWidth, &textHeight);
      _display.setCursor(_display.width() / 2 - textWidth / 2, _display.height() / 2 - textHeight / 2);
      _display.print(GAME_OVER_TEXT);
      
      if (left_btn.wasPressed() || right_btn.wasPressed()) {
        gameState = PLAY;
        setupPlayState();
        timeLeft = 60000 - 5000;
      }
      break;
  }

  prevState = gameState;

  _display.display();
  _tone32.update();
}
