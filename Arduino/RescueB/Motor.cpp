#include "Arduino.h"
#include "Configuration.h"
#include "Motor.h"

// Constructor
Motor::Motor(){
}

void Motor::init(){
    // init motor pins. 
    pinMode(M1,OUTPUT);
    pinMode(E1,OUTPUT);
    pinMode(E2,OUTPUT);
    pinMode(M2,OUTPUT);
}

void Motor::stop() {
  analogWrite(E1,0);
  analogWrite(E2,0);
}

void Motor::brake(){
  int lpower = lastLpower > 0 ? -255 : 255;
  int rpower = lastRpower > 0 ? -255: 255;
  motorPower(lpower, rpower);
  delay(20);
  stop();
}

void Motor::motorPower(int lpower, int rpower) {
  if(lpower < 0) digitalWrite(M1, LOW);
  else digitalWrite(M1, HIGH);
  if(rpower < 0) digitalWrite(M2, LOW);
  else digitalWrite(M2, HIGH);
    
  analogWrite(E1, abs(lpower));
  analogWrite(E2, abs(rpower));
  lastLpower = lpower;
  lastRpower = rpower;
}

void Motor::turnToHeading(float currentHeading, float targetHeading){
    // determine error
  float error = currentHeading-targetHeading;
  if(error > PI) {
    error -= 2*PI;
    //Serial.println("error > PI");
  } else if(error < -PI) {
    error += 2*PI;
    //Serial.println("error < -PI");
  }
  // Linear - Calculate Power
  // Determine direction of rotation
  int power = rotationalPower(error);
  if(error>0){
    motorPower(-power, power);
  }else{
    motorPower(power, -power);
  }
}

int Motor::rotationalPower(float error) {
  int x = abs(error) * ROTATIONAL_SLOWING_CONSTANT + MINIMUM_ROTATIONAL_POWER;
  if(x>MAXIMUM_ROTATIONAL_POWER) { //if bigger than maximun voltage power
    x = MAXIMUM_ROTATIONAL_POWER;
  }
  return x;
}

void Motor::travelPower(float error) {
  int power = error * FORWARD_SLOWING_CONSTANT + 
    (error>0? MINIMUM_FORWARD_POWER: -MINIMUM_FORWARD_POWER);

  if(power>MAXIMUM_FORWARD_POWER) {
    power = MAXIMUM_FORWARD_POWER;
  }else if (power<-MAXIMUM_FORWARD_POWER){
    power = -MAXIMUM_FORWARD_POWER;
  }
  /*
  Serial.print("Travel Power: \t");
  Serial.print(error);
  Serial.print("\t");
  Serial.println(power);
  */
  motorPower(power,power);
}












