// Gavin copy

#include <QueueList.h>
#include <StackArray.h>

#include "Configuration.h"

#include <Servo.h>
#include "Sensors.h"
#include "Motor.h"
#include "Map.h"
#include <Wire.h>

Sensors sensors;
Motor motor;

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
  motor.init();

  Serial.println(F("Sensor Init complete"));
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  
  delay(1000);

  //Map cmap(MAP_WIDTH, MAP_HEIGHT);
  //cmap.printMap();

  //Map::debugMap(8,4, 0, 0);

/*
  turnToBearing(NORTH);
  delay(2000);
  turnToBearing(SOUTH);
  delay(2000);
  turnToBearing(EAST);
  delay(2000);
  turnToBearing(WEST);
  delay(2000);
*/

  forwardOneTile();
}

void loop(){
  Serial.println(sensors.getRange());
  delay(500);
}

void turnToBearing(float targetHeading){
  float curHeading = sensors.getHeading();
  Serial.println(curHeading);
  while( abs(curHeading - targetHeading) > HEADING_TOLERANCE ){
    motor.turnToHeading(curHeading, targetHeading);
    delay(10);
    curHeading = sensors.getHeading();
    Serial.println(curHeading);
  }
  motor.stop();
}

void proceedTo(float heading){
  turnToBearing(heading);
  delay(1000);
  forwardOneTile();
  delay(1000);
}

void forwardOneTile(){
  float currentRange = sensors.getRange();

  //Assume tiles from wall > 0 
  int tilesFromWall = currentRange / 300;

  Serial.println(tilesFromWall);
  
  float targetRange = (tilesFromWall - 1) * 300 + FORWARD_CLEARANCE;
  float error = currentRange - targetRange;
  while( abs(error) > FORWARD_TOLERANCE) {
    motor.travelPower(error);
    currentRange = sensors.getRange();
    error = currentRange - targetRange;
  }

  motor.stop();
}

