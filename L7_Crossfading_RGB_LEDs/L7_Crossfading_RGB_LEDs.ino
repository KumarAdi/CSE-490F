#include "src/RGBConverter/RGBConverter.h"

const int DELAY_MS = 5;
const int RGB_RED_LED_PIN = 6;
const int RGB_GREEN_LED_PIN = 5;
const int RGB_BLUE_LED_PIN = 3;

// RGB Crossfading state

//const int MAX_COLOR_VAL = 255;  
//const int FADE_STEP = 5;
//
//int _rgbLedValues[] = {255, 0, 0};
//
//typedef enum {
//  RED,
//  GREEN,
//  BLUE,
//  NUM_COLORS
//} RGB;
//
//RGB _curFadingUpColor = GREEN;
//RGB _curFadingDownColor = RED;

// HSL Crossfading State

const float SATURATION = 1;
const float LIGHTNESS = 0.5f;
float _hue = 0; //hue varies between 0 - 1
const float STEP = 0.001f;

RGBConverter _rgbConverter;


void setRgbLedColor(int red, int green, int blue){
  Serial.print(red);
  Serial.print(", ");
  Serial.print(green);
  Serial.print(", ");
  Serial.println(blue);
  
  digitalWrite(RGB_RED_LED_PIN, red);
  digitalWrite(RGB_GREEN_LED_PIN, green);
  digitalWrite(RGB_BLUE_LED_PIN, blue);
}

void setup() {
  Serial.begin(9600);
  
  // put your setup code here, to run once:
  pinMode(RGB_RED_LED_PIN, OUTPUT);
  pinMode(RGB_BLUE_LED_PIN, OUTPUT);
  pinMode(RGB_GREEN_LED_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  // RGB crossfading logic

//  _rgbLedValues[_curFadingUpColor] += FADE_STEP;
//  _rgbLedValues[_curFadingDownColor] -= FADE_STEP;
//
//  if (_rgbLedValues[_curFadingUpColor] >= MAX_COLOR_VAL) {
//    _rgbLedValues[_curFadingUpColor] = MAX_COLOR_VAL;
//    _curFadingUpColor = (RGB)((int)_curFadingUpColor + 1);
//
//    if(_curFadingUpColor > (int)BLUE){
//      _curFadingUpColor = RED;
//    }
//  }
//
//  if (_rgbLedValues[_curFadingDownColor] <= 0) {
//    _rgbLedValues[_curFadingDownColor] = 0;
//    _curFadingDownColor = (RGB)((int)_curFadingDownColor + 1);
//
//    if(_curFadingDownColor > (int)BLUE){
//      _curFadingDownColor = RED;
//    }
//  }

//  setRgbLedColor(_rgbLedValues[RED], _rgbLedValues[GREEN], _rgbLedValues[BLUE]);

  // HSL crossfading Logic

  _hue += STEP;
  if(_hue > 1.0){
    _hue = 0;
  }

  byte rgb[3];
  _rgbConverter.hslToRgb(_hue, SATURATION, LIGHTNESS, rgb);

  setRgbLedColor(rgb[0], rgb[1], rgb[2]); 

  delay(DELAY_MS);
}
