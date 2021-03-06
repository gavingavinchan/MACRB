// Gavin copy

#include <QueueList.h>
#include <StackArray.h>

#include "Configuration.h"

#include <Servo.h>
#include "Sensors.h"
#include "Map.h"
#include <Wire.h>

Direction currentDirection = North;
Coordinate currentPos, entrance;
Map cmap(MAP_WIDTH, MAP_HEIGHT);

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

  
  //cmap.printMap();

  //Map::debugMap(8,4, 0, 0);

  // Initial Position
  entrance.x = ENTRANCEX;
  entrance.y = ENTRANCEY;
  
  currentPos.x = ENTRANCEX;
  currentPos.y = ENTRANCEY;
  
}

/*
 * main Algorithm
 */
void mainAlgorithm() {
  while(//all tiles visited and back at start) {
    //tile condition fuction
    tileCondition();
    
    //set current tile as visited
    cmap.addVisit(currentPos);
    
    //determine destination function
    // Path stack determine the complete path to go
    StackArray<Coordinate> pathStack = cmap.findPath(currentPos, currentDirection, entrance);

    // The top of the stack should always be the node itself
    if (pathStack.count()<=1) {
      Serial.println("NO MORE NODE TO GO");
      break;
    }

    pathStack.pop(); // discard since this is the current position
    Coordinate next = pathStack.pop();
    Serial.print(F("Next destination"));
    rmap.printCoordinate(next);
    Serial.println();
    cmap.printMap();
    
    //go to destination
    float dir = MAP_NORTH;
    if (next.x > currentPos.x) dir = MAP_EAST;
    else if (next.y > currentPos.y) dir = MAP_SOUTH;
    else if (next.x < currentPos.x) dir = MAP_WEST;
    else dir = MAP_NORTH;
    moveOneTile(dir);
    
  }
}
  
/*
 * tile condition fuction
 */
void tileCondition() {
boolean FWall;
boolean LWall;
boolean RWall;
    if(Sensors.getRange() < DETERMINE_WALL_DISTANCE) {
      FWall = true;
    if(Sensors.getIrDistance(DIST_LEFT_PIN) < DETERMINE_WALL_DISTANCE) {
      LWall = true;
    }
    if(Sensors.getIrDistance(DIST_RIGHT_PIN) < DETERMINE_WALL_DISTANCE) {
      RWall = true;
  }
  
  if(currentDirection==North) {    // Heading is north
    if(FWall) cmap.setWall(currentPos, North);      // North wall
    
    if(LWall) cmap.setWall(currentPos, West);       // west wall
    
    if(RWall) cmap.setWall(currentPos, East);      // East wall
    
  }else if(currentDirection==EAST) {    // heading east
    if(FWall) cmap.setWall(currentPos, East);      // East wall
    
    if(LWall) cmap.setWall(currentPos, North);      // North wall
    
    if(RWall) cmap.setWall(currentPos, South);      // south wall

  } else if(currentDirection==SOUTH) {   // heading south
    if(FWall) cmap.setWall(currentPos, South);      // South wall

    if(LWall) cmap.setWall(currentPos, East);      // East wall

    if(RWall) cmap.setWall(currentPos, West);      // West wall

  } else if(currentDirection==WEST) {   // heading west
    if(FWall) cmap.setWall(currentPos, West);      // West wall
    
    if(LWall) cmap.setWall(currentPos, South);      // South wall
    
    if(RWall) cmap.setWall(currentPos, North);      // North wall


}

void moveOneTile(float heading) {
  while(//at destination? ) {
    if(Sensors.getTemperatureLeft > 28 || Sensors.getTemperatureRight > 28) {   // if found victim
      //stop();
      for(int i;i<=5;i++) {
        digitalWrite(LED_PIN,HIGH);
        delay(500);
        digitalWrite(LED_PIN,LOW);
        delay(500);
      }
      //mark victim
    }
    //continue towards destination
  }
}

/*
 * Detect black tiles
 */
void blackTile() {
  if(/*black sensor on  */) {
    //stop();
    //mark black tile
    //retreat to previous tile
    //set new destination
  }
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
