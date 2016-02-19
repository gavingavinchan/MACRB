#include "Arduino.h"
#include "Configuration.h"

#ifndef Motor_h
#define Motor_h

class Motor{
  public:
    Motor();

    void init();
    // +ve power -> forward, -ve power -> backward
    void motorPower(int lpower, int rpower);
    
    void stop();
    void left(char a,char b);
    void right(char a,char b);

    // Determines the power required for the motor to turn to the specific heading, it does not stop the car when it arrives
    void turnToHeading(float currentHeading, float targetHeading);
    void travelPower(float error);
  private:
    int rotationalPower(float error);
};

#endif
