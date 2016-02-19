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

    if(Sensors.getRange() < DETERMINE_WALL_DISTANCE) {
      boolean RangeWall = true;
    if(Sensors.getIrDistance(DIST_LEFT_PIN) < DETERMINE_WALL_DISTANCE) {
      boolean IRLeftWall = true;
    }
    if(Sensors.getIrDistance(DIST_RIGHT_PIN) < DETERMINE_WALL_DISTANCE) {
      boolean IRRightWall = true;
  }
  
  if(currentDirection==North) {    // Heading is north
    if(RangeWall) {
      // north wall
      cmap.setWall(currentPos, North);
    }
    if(IRLeftWall) {
      // west wall
    }
    if(IRRightWall==true) {
      // east wall
    }
  }else if(currentDirection==EAST) {    // heading east
    if(RangeWall==true) {
      // East wall
    }
    if(IRLeftWall==true) {
      // north wall
    }
    if(IRRightWall==true) {
      // south wall
    }
  } else if(currentDirection==SOUTH) {   // heading south
    if(RangeWall==true) {
      // South wall
    }
    if(IRLeftWall==true) {
      // East wall
    }
    if(IRRightWall==true) {
      // West wall
    }
  } else if(currentDirection==WEST) {   // heading west
    if(RangeWall==true) {
      // South wall
    }
    if(IRLeftWall==true) {
      // East wall
    }
    if(IRRightWall==true) {
      // West wall
    }

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
