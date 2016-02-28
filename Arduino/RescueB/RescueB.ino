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
Servo kitDispenser;

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

  kitDispenser.attach(KIT_SERVO);
  kitDispenser.writeMicroseconds(KIT_STANDBY_MS);

  pinMode(LED_PIN, OUTPUT);
  for(int i=0; i<3; i++){
    digitalWrite(LED_PIN, HIGH);
    delay(300);
    digitalWrite(LED_PIN, LOW);
    delay(300);
  }
  doRescueTask();
}

void loop(){
  printAllSensorValues();
  delay(500);
}

boolean frontWall(){
  return sensors.getRange()<DETERMINE_WALL_DISTANCE;
}

boolean leftWall () {
  float dist = sensors.getIrDistance(DIST_FL_PIN,3);
  return dist<DETERMINE_WALL_DISTANCE && dist>0;
}

boolean rightWall() {
  float dist = sensors.getIrDistance(DIST_FR_PIN,3);
  return dist<DETERMINE_WALL_DISTANCE && dist>0;
}

boolean blackTile() {
  if(sensors.getGray()>BLACK_THRESHOLD) {
    return true;
  }
  return false;
}

int vict = 0;

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
    cmap.printMap();
    StackArray<Coordinate> pathStack = cmap.findPath(robotPosition, robotOrientation, entrance);
    if (pathStack.count()<=1) break;
    pathStack.pop(); // current position; no need
    Coordinate next = pathStack.pop();
    Serial.print(F("New destination"));
    cmap.printCoordinate(next);
    Serial.println();

    // ********************************************
    // ********DEBUG*****************************
    //while(Serial.read()!='n'){}   //enter n to control each step

    Direction nextDir;
    if (next.x > robotPosition.x){
      // Next is on the East
      nextDir = East;
    }else if (next.y > robotPosition.y){
      nextDir = South;
    }else if (next.x < robotPosition.x){
      nextDir = West;
    }else if (next.y < robotPosition.y){
      nextDir = North;
    }else{
      // Finished
      
      break;
    }

    float heading = MAP_NORTH;
    switch(nextDir){
      case South: heading = MAP_SOUTH; break;
      case East: heading = MAP_EAST; break;
      case West: heading = MAP_WEST; break;
    }
    
    // turnToBearing(heading);
    // Rotate car and detect victim while rotating
    float curHeading = sensors.getHeading();
    while( abs(curHeading - heading) > HEADING_TOLERANCE ){
      motor.turnToHeading(curHeading, heading);
      delay(10);
      curHeading = sensors.getHeading();
      // detect Victim chunk
      if (!cmap.hasVictim(robotPosition)){
        vict = detectVictim();
        if(vict > 0) {
          Serial.println("Victim found while rotating");
          // victimFound(vict,nextDir);
          cmap.setVictim(robotPosition);
          motor.brake();
          float curBearing = sensors.getHeading();
          float victBearing = curBearing;
          if(vict==1) victBearing += PI/2;
          else victBearing -= PI/2;
          turnToBearing(victBearing);
          victimFound();
        } 
      }
    }
    motor.brake();
    delay(200);
    

    delay(200);


    // Forward One Tile and detect victim
    boolean blackDetected = false;
    boolean moveBack = false;
    float initialRange = sensors.getRange();
    while(initialRange>3000){
      motor.brake();
      initialRange = sensors.getRange();
    }
    Serial.print("initialRange: ");
    Serial.println(initialRange);

    float currentRange = initialRange;
    
    //Assume tiles from wall > 0 
    int tilesFromWall = currentRange / 300;
  
    //Serial.println(tilesFromWall);
    
    float targetRange = (tilesFromWall - 1) * 300 + FORWARD_CLEARANCE;
    float error = currentRange - targetRange;
    Coordinate movingPosition = robotPosition;
    
    while( abs(error) > FORWARD_TOLERANCE) {
      motor.travelPower(error);
      Serial.print("tilesFromWall ");
      Serial.println(tilesFromWall);
      if(currentRange < tilesFromWall * 300){
        Serial.println("Change to next");
        movingPosition = next;
      }else{
        movingPosition = robotPosition;
      }

      // check black tile
      if(!blackDetected && blackTile()){
        Serial.println("blackTile detected");
        blackDetected = true;
        if (movingPosition.x!=entrance.x || movingPosition.y!=entrance.y){
          targetRange = initialRange;
          cmap.setBlackTile(next);
          moveBack = true;
        }
      }
      
      if (!cmap.hasVictim(movingPosition)){
        vict = detectVictim();

        if(vict > 0) {
          Serial.println("Victim found while moving forward");
          // victimFound(vict,nextDir);
          cmap.setVictim(movingPosition);
          motor.brake();
          float curBearing = sensors.getHeading();
          float victBearing = curBearing;
          if(vict==1) victBearing += PI/2;
          else victBearing -= PI/2;
          turnToBearing(victBearing);
          victimFound();
          turnToBearing(curBearing);
          motor.brake();
        } 
      }
    
      currentRange = sensors.getRange();
      if(currentRange<3000){
        Serial.print(currentRange);
        Serial.print("\t");
        Serial.println(targetRange);
        error = currentRange - targetRange;
      }
    }
    motor.brake();
        /*
        if(forwardOneTile(
          !(
            (entrance.x==robotPosition.x&&entrance.y==robotPosition.y) ||       //remove if statemet if entrance is not black
            (entrance.x==next.x&&entrance.y==next.y)) )) {
          cmap.setBlackTile(next);
          robotOrientation = nextDir;
          continue;
        }
        */   
    delay(500);

    if(!moveBack)
      robotPosition = next;
    robotOrientation = nextDir;
  }while(true);

  Serial.println("GOAL");
}

void turnToBearing(float targetHeading){
  float curHeading = sensors.getHeading();
  if(targetHeading<0) targetHeading += 2*PI;
  else if (targetHeading>= 2*PI) targetHeading -= 2*PI;
  //Serial.println(curHeading);
  while( abs(curHeading - targetHeading) > HEADING_TOLERANCE ){
    motor.turnToHeading(curHeading, targetHeading);
    delay(10);
    curHeading = sensors.getHeading();
    //Serial.println(curHeading);
  }
  motor.brake();
}

void printAllSensorValues(){
  Serial.println("IR_FL\tIR_FR\tIR_BL\tIR_BR\tRan\tCom\tTempLeft\tTempRight\tCGPGrey");
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
  Serial.print(sensors.getTemperatureLeft());
  Serial.print("\t\t");
  Serial.print(sensors.getTemperatureRight());
  Serial.print("\t\t");
  Serial.print(sensors.getGray());
  Serial.print("\t");
  Serial.println();
}


float mapOrientation(Direction dir) {
  float heading = MAP_NORTH;
  switch(dir){
    case South: heading = MAP_SOUTH; break;
    case East: heading = MAP_EAST; break;
    case West: heading = MAP_WEST; break;
  }
  return heading;
}

void setWall(Map &cmap, Coordinate currentPos, Direction currentDirection) {
  boolean FWall = false, LWall = false, RWall = false;
  if(frontWall()) {
    FWall = true;
  }
  if(leftWall()) {
    LWall = true;
  }
  if(rightWall()) {
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

// return int
// 0 - not found
// 1 - left 
// 2 - right
int detectVictim(){
  if (sensors.getTemperatureLeft() > VICTIM_TEMP && leftWall()) return 1;
  if (sensors.getTemperatureRight() > VICTIM_TEMP && rightWall()) return 2;

  return 0;
}

// What to do after victim is located
void victimFound(){
  Serial.println("Victim FOUND");
  
  // float previousMapOrientation = mapOrientation(robotOrientation);
  //flash for 5 sec while dispensing kit
  kitDispenser.writeMicroseconds(KIT_OPEN_MS);
  digitalWrite(LED_PIN,HIGH);
  delay(500);
  digitalWrite(LED_PIN,LOW);
  delay(500);
  kitDispenser.writeMicroseconds(KIT_DISPATCH_MS);
  digitalWrite(LED_PIN,HIGH);
  delay(500);
  digitalWrite(LED_PIN,LOW);
  delay(500);
  kitDispenser.writeMicroseconds(KIT_STANDBY_MS);
  digitalWrite(LED_PIN,HIGH);
  delay(500);
  digitalWrite(LED_PIN,LOW);
  delay(500);
  digitalWrite(LED_PIN,HIGH);
  delay(500);
  digitalWrite(LED_PIN,LOW);
  delay(500);
  digitalWrite(LED_PIN,HIGH);
  delay(500);
  digitalWrite(LED_PIN,LOW);
  delay(250);
  digitalWrite(LED_PIN,HIGH);
  delay(250);
  digitalWrite(LED_PIN,LOW);
  /*
  float victimDirection = MAP_NORTH;
  if(vict == 2) {   //opposite direction since point butt at victim not head
    switch(robotOrientation) {
      case North:
        victimDirection = MAP_WEST;
        break;
      case South:
        victimDirection = MAP_EAST;
        break;
      case West:
        victimDirection = MAP_SOUTH;
        break;
    }
  } else if(vict == 1) {
    switch(robotOrientation) {
      case North:
        victimDirection = MAP_EAST;
        break;
      case South:
        victimDirection = MAP_WEST;
        break;
      case East:
        victimDirection = MAP_SOUTH;
        break;
    }
  }
  */
  //Serial.println("turnToBearing(victimDirection)");
  // turnToBearing(buttBearing);
  //dispense kit

  
  //turn back to previous orientation
  //Serial.println("turnToBearing(previousMapOrientation)");
  // turnToBearing(previousMapOrientation);
}

