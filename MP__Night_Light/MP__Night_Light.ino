#include <CapacitiveSensor.h>
#include "src/JC_Button/JC_Button.h"
#include "src/RGBConverter/RGBConverter.h"

const bool LOG_SENSOR_VALUES = false;
const bool LOG_INFO = true;
const bool LOG_OUTPUT_VALUES = false;

const int MODE_BUTTON_PIN = 4;
Button mode_btn(MODE_BUTTON_PIN, 50);

// CS = Capacitive sensing

const int CS_TRANSMIT_PIN = 13;
const int CS_RECEIVE_PIN = 13;

// The CS threshold needs to be set based on environment

// OLD SENSING SCHEME:
// const long CS_THRESHOLD = 2000; // in my room, touches are indicated by values > than this threshold when outside any casing
//const long CS_THRESHOLD = 40000; // in the basement, touches are indicated by values < this threshold when outside any casing
//const long CS_THRESHOLD = 90000; // in the basement, touches are indicated by values < this threshold when inside the prototype casing

// NEW SENSING SCHEME
const long CS_THRESHOLD = 300; // in the basement, touches are indicated by values < than this threshold when inside the final casing

CapacitiveSensor cs = CapacitiveSensor(CS_TRANSMIT_PIN,CS_RECEIVE_PIN);

const int PHOTORES_PIN = A0;
const float PHOTORES_MIN = 0;
const float PHOTORES_MAX = 1000;

const int RGB_RED_LED_PIN = 3;
const int RGB_GREEN_LED_PIN = 5;
const int RGB_BLUE_LED_PIN = 6;
const float SATURATION_STEP = 0.1f;
RGBConverter _rgbConverter;

void setRgbLedColor(int red, int green, int blue){
  if (LOG_OUTPUT_VALUES) {
    Serial.print(red);
    Serial.print(", ");
    Serial.print(green);
    Serial.print(", ");
    Serial.println(blue);
  }
  analogWrite(RGB_RED_LED_PIN, red);
  analogWrite(RGB_GREEN_LED_PIN, green);
  analogWrite(RGB_BLUE_LED_PIN, blue);
}

float photoResToLightness(float photoVal) {
  return (photoVal - PHOTORES_MIN) / (PHOTORES_MAX - PHOTORES_MIN);
}

float messageToHue (String message) {
  if (message.equalsIgnoreCase("red\n")) {
    return 0.0f;
  } else if (message.equalsIgnoreCase("blue\n")) {
    return 0.5f;
  } else {
    Serial.println("I'm sorry, I don't recognize that color");
  }
}

void setup() {
  cs.set_CS_AutocaL_Millis(0xFFFFFFFF);
  Serial.begin(9600); 

  pinMode(PHOTORES_PIN, INPUT);
  pinMode(RGB_RED_LED_PIN, OUTPUT);
  pinMode(RGB_BLUE_LED_PIN, OUTPUT);
  pinMode(RGB_GREEN_LED_PIN, OUTPUT);
  mode_btn.begin();
}

float hue = 0.5;
float saturation = 0;
unsigned long timeOfLastTouch = millis();

typedef enum {
  CROSS_FADE = 0,
  LO_FI = 1,
  SPECIAL = 2,
} LampMode;

LampMode mode = CROSS_FADE;

void loop() {
  float ledBrightness = 0.5f;

  mode_btn.read();
  if (mode_btn.wasPressed()) {
    mode = (LampMode)((int) mode + 1) % 3;
    if (LOG_INFO) {
      Serial.print("Changing mode to ");
      Serial.println(mode);      
    }
  }

  if (mode == CROSS_FADE) {
    int photoVal = analogRead(PHOTORES_PIN);
    ledBrightness = 1 - photoResToLightness(photoVal);
    if (LOG_SENSOR_VALUES) {
      Serial.print("photoVal:");
      Serial.println(photoVal);      
    }

    if (LOG_INFO) {
      Serial.print("ledBrightness:");
      Serial.println(ledBrightness);
    }
  }

  if (mode == LO_FI) {
    long touchVal =  millis();
    cs.capacitiveSensor(60);
    touchVal = millis() - touchVal;
    if (LOG_SENSOR_VALUES) {
      Serial.print("touchVal:");
      Serial.println(touchVal);
    }

    unsigned long currentMillis = millis();
    unsigned long timeSinceLastTouch = currentMillis - timeOfLastTouch;
    if (touchVal < CS_THRESHOLD && timeSinceLastTouch > 250) {
      timeOfLastTouch = currentMillis;
      saturation += SATURATION_STEP;
      if (saturation > 1) {
        saturation = 1;
      } else if (LOG_INFO) {
        Serial.print("Saturation increasing to ");
        Serial.println(saturation);
      }
    } else if (timeSinceLastTouch > 1000) {
      saturation -= SATURATION_STEP;
      if (saturation < 0) {
        saturation = 0;
      } else if (LOG_INFO) {
        Serial.print("Saturation decreasing to ");
        Serial.println(saturation);
      }
    }
  } else {
    saturation = 0.5;
  }

  if (mode == SPECIAL) {
    if (Serial.available()) {
      String message = Serial.readString();
      if (LOG_SENSOR_VALUES) {
        Serial.println(message);
      }
      hue = messageToHue(message);
      if (LOG_INFO) {
        Serial.print("Setting hue to ");
        Serial.print(hue);
      }
    }
  } else {
    hue = 0.0f;
  }

  byte rgb[3];
  _rgbConverter.hslToRgb(hue, saturation, ledBrightness, rgb);

  setRgbLedColor(rgb[0], rgb[1], rgb[2]); 
}
