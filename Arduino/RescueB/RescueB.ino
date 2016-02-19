// Gavin copy

#include <QueueList.h>
#include <StackArray.h>

#include "Configuration.h"

#include <Servo.h>
#include "Sensors.h"
#include "Map.h"
#include <Wire.h>

Sensors sensors;
int mode = 1;
float initOrientation = 0;
boolean rotate = false;
void setup(){
  // Hardware bus init
  Wire.begin();
  
  Serial.begin(115200);
  delay(100);
  Serial.println(F("START"));
  delay(500);

  Serial.println(F("Sensor Initialization"));
  sensors.init();

  Serial.println(F("Sensor Init complete"));
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  
  delay(1000);

  //Map cmap(MAP_WIDTH, MAP_HEIGHT);
  //cmap.printMap();

  //Map::debugMap(8,4, 0, 0);
}

/*
 * main Algorithm
 */
while(//all tiles visited and back at start) {
  //tile condition fuction

  //set current tile as visited

  //determine destination function

  //go to destination
}

/*
 * tile condition fuction
 */
while() {
  if(Sensors.getRange() < ) {
}

void loop(){
  Serial.print("heading\t");
  Serial.println(sensors.getHeading());
  
  Serial.print("Ultra Sound\t");
  Serial.println(sensors.getRange());

  Serial.print("Temperature Right\t");
  Serial.println(sensors.getTemperatureRight());
  
  Serial.print("Temperature Left\t");
  Serial.println(sensors.getTemperatureLeft());
  
  Serial.print("IR Left\t");
  Serial.println(sensors.getIrDistance(DIST_LEFT_PIN, 1));
  
  Serial.print("IR Right\t");
  Serial.println(sensors.getIrDistance(DIST_RIGHT_PIN, 1));

  delay(1000);
}
