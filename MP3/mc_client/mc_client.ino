#include "Secrets.h"

#include <WiFi.h>
#include <HTTPClient.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

// temporary test server
String serverURL = "http://192.168.1.13:3000/start";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 

  WiFi.begin(ssid, password);

  if(!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  _display.clearDisplay();
  _display.setTextSize(1);      // Normal 1:1 pixel scale
  _display.setTextColor(SSD1306_WHITE); // Draw white text
  _display.setCursor(0, 0);     // Start at top-left corner
  
  _display.print("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    _display.print(".");
    _display.display();
  }
  _display.println("");
  _display.clearDisplay();
  _display.print("Connected to WiFi network with IP Address: ");
  _display.println(WiFi.localIP());
  _display.display();
 
  HTTPClient http;
  http.begin(serverURL.c_str());
  int httpResponseCode = http.POST("");

  if (httpResponseCode > 0) {
    _display.clearDisplay();
    _display.print("HTTP Response code: ");
    _display.println(httpResponseCode);
    String payload = http.getString();
    _display.println(payload);
    _display.display();
  }
  else {
    _display.print("Error code: ");
    _display.println(httpResponseCode);
  }

  http.end();
}

void loop() {
  // put your main code here, to run repeatedly:

}
