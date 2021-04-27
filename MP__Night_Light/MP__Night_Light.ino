#include <CapacitiveSensor.h>

const int csThreshold = 2000;

CapacitiveSensor cs = CapacitiveSensor(13,12);

void setup() {
  // put your setup code here, to run once:
  cs.set_CS_AutocaL_Millis(0xFFFFFFFF);
  Serial.begin(9600); 

}

void loop() {
  // put your main code here, to run repeatedly:
  long sensedVal =  cs.capacitiveSensor(60);
//  Serial.print("sensedVal:");
//  Serial.println(sensedVal);
  if (sensedVal > csThreshold) {
    Serial.println("pressed");
  }
}
