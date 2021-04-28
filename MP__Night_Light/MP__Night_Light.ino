#include <CapacitiveSensor.h>
#include "src/RGBConverter/RGBConverter.h"

const int CS_TRANSMIT_PIN = 13;
const int CS_RECEIVE_PIN = 13;
const int CS_THRESHOLD = 2000;
CapacitiveSensor cs = CapacitiveSensor(CS_TRANSMIT_PIN,CS_RECEIVE_PIN);

const int PHOTORES_PIN = A0;

const int RGB_RED_LED_PIN = 3;
const int RGB_GREEN_LED_PIN = 5;
const int RGB_BLUE_LED_PIN = 6;
const float SATURATION = 0;
const float LIGHTNESS = 1.0f;
RGBConverter _rgbConverter;

void setRgbLedColor(int red, int green, int blue){
  digitalWrite(RGB_RED_LED_PIN, red);
  digitalWrite(RGB_GREEN_LED_PIN, green);
  digitalWrite(RGB_BLUE_LED_PIN, blue);
}

void setup() {
  cs.set_CS_AutocaL_Millis(0xFFFFFFFF);
  Serial.begin(9600); 

  pinMode(PHOTORES_PIN, INPUT);
  pinMode(RGB_RED_LED_PIN, OUTPUT);
  pinMode(RGB_BLUE_LED_PIN, OUTPUT);
  pinMode(RGB_GREEN_LED_PIN, OUTPUT);
}

void loop() {
  long touchVal =  cs.capacitiveSensor(60);
//  Serial.print("touchVal:");
//  Serial.println(touchVal);
  
  int photoVal = analogRead(PHOTORES_PIN);
  Serial.print("photoVal:");
  Serial.println(photoVal);

  byte rgb[3];
  _rgbConverter.hslToRgb(0.5, SATURATION, LIGHTNESS, rgb);

  setRgbLedColor(rgb[0], rgb[1], rgb[2]); 

  
  if (touchVal > CS_THRESHOLD) {
    Serial.println("pressed");
  }
}
