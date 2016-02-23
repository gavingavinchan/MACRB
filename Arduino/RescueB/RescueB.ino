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
Direction robotOrientation = North;
Coordinate robotPosition, entrance;

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

  // doRescueTask();
  testRotation2();
}

void loop(){
  printAllSensorValues();
  delay(500);
}

boolean frontWall(){
  return sensors.getRange()<DETERMINE_WALL_DISTANCE;
}

boolean leftWall () {
  return sensors.getIrDistance(DIST_FL_PIN,3)<DETERMINE_WALL_DISTANCE;
}

boolean rightWall() {
  return sensors.getIrDistance(DIST_FR_PIN,3)<DETERMINE_WALL_DISTANCE;
}

void testRotation2(){
  // determine to use left or right sensors
  char fpin, bpin;
  boolean trustTheCompass = false;
  if (frontWall()){
    fpin = DIST_FL_PIN;
    bpin = DIST_BL_PIN;
    Serial.println("use Left");
  }else if(leftWall() || rightWall()){
    Serial.println("trust the compass");
    trustTheCompass = true;
  }else{
    fpin = DIST_BR_PIN;
    bpin = DIST_FR_PIN;
    Serial.println("use Right");
  }
  // get current heading and +90 degrees
  float heading = sensors.getHeading() + PI/2;
  heading = heading > 2*PI ? heading - 2*PI: heading;
  turnToBearing(heading);
  motor.stop();
  if(trustTheCompass) {
    return;
  }
  delay(1000);
  
  float irfl = sensors.getIrDistance(fpin, 1);
  float irbl = sensors.getIrDistance(bpin, 1);
  if (irfl<0) irfl = 500;
  if (irbl<0) irbl = 500;
  float error = irfl - irbl;
  while(abs(error)>1){
    if(error>0){
      motor.left(160, 160);
    }
    else{
      motor.right(160, 160);
    }
    
    delay(20);
    irfl = sensors.getIrDistance(fpin, 1);
    irbl = sensors.getIrDistance(bpin, 1);
    if (irfl<0) irfl = 500;
    if (irbl<0) irbl = 500;
    error =   (irfl - irbl);
  }
  motor.stop();
}

void doRescueTask(){
  // Initalize map and global variable on the task
  Map cmap(MAP_WIDTH, MAP_HEIGHT);
  entrance.x = ENTRANCEX;
  entrance.y = ENTRANCEY;
  robotPosition.x = ENTRANCEX;
  robotPosition.y = ENTRANCEY;
  robotOrientation = North;

  do{
    printAllSensorValues();
    setWall(cmap, robotPosition, robotOrientation);
    cmap.addVisit(robotPosition);
    //cmap.printMap();
    StackArray<Coordinate> pathStack = cmap.findPath(robotPosition, robotOrientation, entrance);
    if (pathStack.count()<=1) break;
    pathStack.pop(); // current position; no need
    Coordinate next = pathStack.pop();
    Serial.print(F("New destination"));
    cmap.printCoordinate(next);
    Serial.println();

    if (next.x > robotPosition.x){
      // Next is on the East
      robotOrientation = East;
    }else if (next.y > robotPosition.y){
      robotOrientation = South;
    }else if (next.x < robotPosition.x){
      robotOrientation = West;
    }else if (next.y < robotPosition.y){
      robotOrientation = North;
    }else{
      // Finished
      
      break;
    }
    
    while(Serial.read()!='\n'){}
    proceedTo(robotOrientation);
    
    robotPosition = next;
  }while(true);

  Serial.println("GOAL");
}

void turnToBearing(float targetHeading){
  float curHeading = sensors.getHeading();
  //Serial.println(curHeading);
  while( abs(curHeading - targetHeading) > HEADING_TOLERANCE ){
    motor.turnToHeading(curHeading, targetHeading);
    delay(10);
    curHeading = sensors.getHeading();
    //Serial.println(curHeading);
  }
  motor.stop();
}

void printAllSensorValues(){
  Serial.println("IR_FL\tIR_FR\tIR_BL\tIR_BR\tRan\tCom");
  Serial.print(sensors.getIrDistance(DIST_FL_PIN));
  Serial.print("\t");
  Serial.print(sensors.getIrDistance(DIST_FR_PIN));
  Serial.print("\t");
  Serial.print(sensors.getIrDistance(DIST_BL_PIN));
  Serial.print("\t");
  Serial.print(sensors.getIrDistance(DIST_BR_PIN));
  Serial.print("\t");
  Serial.print(sensors.getRange());
  Serial.print("\t");
  Serial.print(sensors.getHeading());
  Serial.print("\t");
  Serial.println();
}

void proceedTo(Direction dir){
  float heading = MAP_NORTH;
  switch(dir){
    case South: heading = MAP_SOUTH; break;
    case East: heading = MAP_EAST; break;
    case West: heading = MAP_WEST; break;
  }
  
  turnToBearing(heading);
  delay(500);
  forwardOneTile();
  delay(500);
}

void forwardOneTile(){
  float currentRange = sensors.getRange();

  //Assume tiles from wall > 0 
  int tilesFromWall = currentRange / 300;

  //Serial.println(tilesFromWall);
  
  float targetRange = (tilesFromWall - 1) * 300 + FORWARD_CLEARANCE;
  float error = currentRange - targetRange;
  while( abs(error) > FORWARD_TOLERANCE) {
    motor.travelPower(error);
    currentRange = sensors.getRange();
    error = currentRange - targetRange;
  }

  motor.stop();
}

void setWall(Map &cmap, Coordinate currentPos, Direction currentDirection) {
  boolean FWall = false, LWall = false, RWall = false;
  float irl = (sensors.getIrDistance(DIST_FL_PIN)+sensors.getIrDistance(DIST_BL_PIN))/2;
  float irr = (sensors.getIrDistance(DIST_FR_PIN)+sensors.getIrDistance(DIST_BR_PIN))/2;
  if(sensors.getRange() < DETERMINE_WALL_DISTANCE) {
    FWall = true;
  }
  if(irl < DETERMINE_WALL_DISTANCE && irl > 0) {
    LWall = true;
  }
  if(irr < DETERMINE_WALL_DISTANCE && irr > 0) {
    RWall = true;
  }
  
  if(currentDirection==North) {    // Heading is north
    if(FWall) cmap.setWall(currentPos, North);      // North wall
    if(LWall) cmap.setWall(currentPos, West);       // west wall
    if(RWall) cmap.setWall(currentPos, East);      // East wall
  }else if(currentDirection==East) {    // heading east
    if(FWall) cmap.setWall(currentPos, East);      // East wall
    if(LWall) cmap.setWall(currentPos, North);      // North wall
    if(RWall) cmap.setWall(currentPos, South);      // south wall
  } else if(currentDirection==South) {   // heading south
    if(FWall) cmap.setWall(currentPos, South);      // South wall
    if(LWall) cmap.setWall(currentPos, East);      // East wall
    if(RWall) cmap.setWall(currentPos, West);      // West wall
  } else if(currentDirection==West) {   // heading west
    if(FWall) cmap.setWall(currentPos, West);      // West wall 
    if(LWall) cmap.setWall(currentPos, South);      // South wall   
    if(RWall) cmap.setWall(currentPos, North);      // North wall
  }
}
