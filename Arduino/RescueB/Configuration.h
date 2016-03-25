#define DEBUGLEVEL  0 // 0 - no debug, 1 - basic, 5 - verbal
#include "Arduino.h"

//Run test mode to find direct values of the N,S,E,W
#define MAP_NORTH 0
#define MAP_SOUTH 3.14
#define MAP_EAST 1.70
#define MAP_WEST 4.3

//Input average Hardiron config
#define HARD_IRON_X 0
#define HARD_IRON_Y 0

#define HEADING_TOLERANCE 0.05
#define FORWARD_TOLERANCE 10

#define FORWARD_CLEARANCE 110

#define FORWARD_SLOWING_DISTANCE 300
#define MINIMUM_FORWARD_POWER 120
#define MAXIMUM_FORWARD_POWER 220
// 255-minimum_foward_power / forward_slowing_distance
#define FORWARD_SLOWING_CONSTANT 0.30

#define ROTATIONAL_SLOWING_DISTANCE 1
#define MINIMUM_ROTATIONAL_POWER 120
#define MAXIMUM_ROTATIONAL_POWER 200
// 255 - minimum_rotational_power / rotation_slowing_distance
#define ROTATIONAL_SLOWING_CONSTANT 50

// MAP MAX WIDTH/HEIGHT must be smaller or less than 15, due to the problem of 4-bit integer
#define MAP_MAX_WIDTH 9
#define MAP_MAX_HEIGHT 4

//Input map width and height, (0,0) is top right
#define  MAP_WIDTH    9
#define  MAP_HEIGHT   4

//Define the entrance block
#define ENTRANCEX  0
#define ENTRANCEY  3

//Define Black Threshold and victim temperature
#define BLACK_THRESHOLD 29
#define VICTIM_TEMP  30

// Motor pin definitions
#define EN1 4

// Software I2C
// MLX90614
#define TEMP_LEFT_ADDR 0x55
#define TEMP_RIGHT_ADDR 0x5C

// HMC5883L - Compass
#define HMC5883L_Address 0x1E
#define HMC5883L_Address_Write 0x3C
#define HMC5883L_Address_Read 0x3D

#define ConfigurationRegisterA 0x00
#define ConfigurationRegisterB 0x01
#define ModeRegister 0x02
#define DataRegisterBegin 0x03

#define Measurement_Continuous 0x00
#define Measurement_SingleShot 0x01
#define Measurement_Idle 0x03

#define Compass_Scale  0.73
#define Compass_Scale_Setting  0x00 << 5

// Ir Distance
#define DIST_FL_PIN  A0
#define DIST_FR_PIN  A1
#define DIST_BL_PIN  A2
#define DIST_BR_PIN  A3

// Ground Gray Sensor
#define GRAY_PIN  2

// Ultrasonic front - TODO
#define KS109  0x74

// Servo for sensor tower
#define SENSOR_SERVO_PIN 11
#define SENSOR_SERVO_CENTER 1586 // microsecond

// Calibration Mode
#define CALIBRATION_MODE_PIN  40

// Timer event trigger millisecond
#define TIMER_SENSOR  200
#define TIMER_DECISION  1000

// determine wall distance
#define DETERMINE_WALL_DISTANCE 200
// Motor pins
#define E1  5
#define M1  4
#define E2  6
#define M2  7

// RESCUE KIT
#define KIT_SERVO 9
#define KIT_OPEN_MS 2200
#define KIT_DISPATCH_MS 1300
#define KIT_STANDBY_MS 1600
// LED
#define LED_PIN  10


