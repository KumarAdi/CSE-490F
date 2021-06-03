#include "Secrets.h"

#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

// temporary test server
String serverURL = "http://192.168.1.13:3000/prompt";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  HTTPClient http;
  http.begin(serverURL.c_str());
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void loop() {
  // put your main code here, to run repeatedly:

}
