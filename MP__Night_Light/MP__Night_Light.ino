#include <CapacitiveSensor.h>
#include "src/RGBConverter/RGBConverter.h"

const int MODE_BUTTON_PIN = 8;

const int CS_TRANSMIT_PIN = 13;
const int CS_RECEIVE_PIN = 13;
// const long CS_THRESHOLD = 2000; // in my room, touches are indicated by values > than this threshold when outside any casing
const long CS_THRESHOLD = 40000; // in the basement, touches are indicated by values < this threshold when outside any casing
//const long CS_THRESHOLD = 90000; // in the basement, touches are indicated by values < this threshold when inside the prototype casing

CapacitiveSensor cs = CapacitiveSensor(CS_TRANSMIT_PIN,CS_RECEIVE_PIN);

const int PHOTORES_PIN = A0;
const float PHOTORES_MIN = 250;
const float PHOTORES_MAX = 850;

const int RGB_RED_LED_PIN = 3;
const int RGB_GREEN_LED_PIN = 5;
const int RGB_BLUE_LED_PIN = 6;
const float SATURATION_STEP = 0.1f;
const float HUE = 0.5f;
RGBConverter _rgbConverter;

void setRgbLedColor(int red, int green, int blue){
//  Serial.print(red);
//  Serial.print(", ");
//  Serial.print(green);
//  Serial.print(", ");
//  Serial.println(blue);
  analogWrite(RGB_RED_LED_PIN, red);
  analogWrite(RGB_GREEN_LED_PIN, green);
  analogWrite(RGB_BLUE_LED_PIN, blue);
}

float photoResToLightness(float photoVal) {
  return (photoVal - PHOTORES_MIN) / (PHOTORES_MAX - PHOTORES_MIN);
}

void setup() {
  cs.set_CS_AutocaL_Millis(0xFFFFFFFF);
  Serial.begin(9600); 

  pinMode(PHOTORES_PIN, INPUT);
  pinMode(RGB_RED_LED_PIN, OUTPUT);
  pinMode(RGB_BLUE_LED_PIN, OUTPUT);
  pinMode(RGB_GREEN_LED_PIN, OUTPUT);
  pinMode(MODE_BUTTON_PIN, INPUT_PULLUP);
}

float saturation = 0;
unsigned long timeOfLastTouch = millis();

enum {
  CROSS_FADE = 0,
  LO_FI = 1,
  SPECIAL = 3,
} mode = CROSS_FADE;

void loop() {
  float ledBrightness = 1;

  if (!digitalRead(MODE_BUTTON_PIN)) {
    mode = (mode + 1) % 2;
  }

  if (mode == CROSS_FADE) {
    int photoVal = analogRead(PHOTORES_PIN);
    ledBrightness = 1 - photoResToLightness(photoVal);
    Serial.print("photoVal:");
    Serial.println(photoVal);
  }

  if (mode == LO_FI) {
    long touchVal =  cs.capacitiveSensor(60);
    Serial.print("touchVal:");
    Serial.println(touchVal);
    
    unsigned long currentMillis = millis();
    unsigned long timeSinceLastTouch = currentMillis - timeOfLastTouch;
    if (touchVal < CS_THRESHOLD && timeSinceLastTouch > 250) {
      timeOfLastTouch = currentMillis;
      saturation += SATURATION_STEP;
      if (saturation > 1) {
        saturation = 1;
      }
    } else if (timeSinceLastTouch > 1000) {
      saturation -= SATURATION_STEP;
      if (saturation < 0) {
        saturation = 0;
      }
    }
  } else {
    saturation = 1;
  }

  byte rgb[3];
  _rgbConverter.hslToRgb(HUE, saturation, ledBrightness, rgb);

  setRgbLedColor(rgb[0], rgb[1], rgb[2]); 
}
