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

void Motor::motorPower(int lpower, int rpower) {
  if(lpower < 0) digitalWrite(M1, LOW);
  else digitalWrite(M1, HIGH);
  if(rpower < 0) digitalWrite(M2, LOW);
  else digitalWrite(M2, HIGH);
    
  analogWrite(E1, abs(lpower));
  analogWrite(E2, abs(rpower));
}

void Motor::left(char a,char b) {
  digitalWrite(M1, LOW);
  digitalWrite(M2, HIGH);
  analogWrite(E1, a);
  analogWrite(E2, b);
}

void Motor::right(char a,char b) {
  digitalWrite(M1, HIGH);
  digitalWrite(M2, LOW);
  analogWrite(E1, a);
  analogWrite(E2, b);
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
  if(error>0){
    left(rotationalPower(error),rotationalPower(error));
  }else{
    right(rotationalPower(error),rotationalPower(error));
  }
}



int Motor::rotationalPower(float error) {
  int x = abs(error) * ROTATIONAL_SLOWING_CONSTANT + MINIMUM_ROTATIONAL_POWER;
  if(x>255) { //if bigger than maximun voltage power
    x = 255;
  }
  return x;
}

void Motor::travelPower(float error) {
  int power = error * FORWARD_SLOWING_CONSTANT + 
    (error>0? MINIMUM_FORWARD_POWER: -MINIMUM_FORWARD_POWER);

  if(power>255) {
    power = 255;
  }else if (power<-255){
    power = -255;
  }
  motorPower(power,power);
}












