#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define FOCAL_LENGTH 64

#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

class Drawable {
  public:
  virtual void draw(Adafruit_SSD1306 _display);
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

  Point3 rotateAbout (Point3 axis, float angle, Point3 origin) {
    *this = origin + (*this - origin).rotate(axis, angle);
  }
  
  void draw(Adafruit_SSD1306 _display) {
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

  void println() {
    Serial.print("(");
    Serial.print(x);
    Serial.print(", ");
    Serial.print(y);
    Serial.print(", ");
    Serial.print(z);
    Serial.println(")");
  }
  
  private:
  float x, y, z;
};

class Line3: Drawable {

  public:
  Line3 (Point3 a, Point3 b) {
    this->a = a;
    this->b = b;
  }
  
  void draw(Adafruit_SSD1306 _display) {
    int x_a, y_a;
    a.toScreenCoords(&x_a, &y_a);
    int x_b, y_b;
    b.toScreenCoords(&x_b, &y_b);

    _display.drawLine(x_a, y_a, x_b, y_b, SSD1306_WHITE);
  }
  
  private:
  Point3 a, b;
};

class Cube3: Drawable {
  public:
  Cube3(Point3 pos, Point3 down, Point3 right, Point3 forward): pos(pos), down(down), right(right), forward(forward) {}

   void draw(Adafruit_SSD1306 _display) {
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
    this->pos = this->pos.rotateAbout(axis, angle, origin);
//    this->rotate(axis, angle);
  }

  void move(Point3 displacement) {
    this->pos = this->pos + displacement;
  }

  private:
  Point3 pos, down, right, forward;
};

Cube3 test_cube(
  Point3(0,0,128),
  Point3(0,16,0),
  Point3(16,0,0),
  Point3(0,0,16)
  );
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!_display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  _display.clearDisplay();
  test_cube.draw(_display);
  test_cube.rotateAbout(Point3(0,1,0), 0.1f, Point3(0, 0, 64));
  _display.display();
}
