#include <CapacitiveSensor.h>

const int CS_TRANSMIT_PIN = 13;
const int CS_RECEIVE_PIN = 13;
const int CS_THRESHOLD = 2000;

const int PHOTORES_PIN = A0;

CapacitiveSensor cs = CapacitiveSensor(CS_TRANSMIT_PIN,CS_RECEIVE_PIN);

void setup() {
  // put your setup code here, to run once:
  cs.set_CS_AutocaL_Millis(0xFFFFFFFF);
  Serial.begin(9600); 

  pinMode(PHOTORES_PIN, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  long touchVal =  cs.capacitiveSensor(60);
//  Serial.print("touchVal:");
//  Serial.println(touchVal);
  
  int photoVal = analogRead(PHOTORES_PIN);
  Serial.print("photoVal:");
  Serial.println(photoVal);

  
  if (touchVal > CS_THRESHOLD) {
    Serial.println("pressed");
  }
}
