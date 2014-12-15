/*
	This class holds all the functionality for controlling the turret and getting
	sensor readings from the range finder and flame sensor, as well as (eventually)
	being able to control the extinguishing mechanism
*/


#ifndef Turret_h
#define Turret_h

#include <Arduino.h>
#include <Servo.h>
#include <Constants.h>
#include <Ultrasonic.h>
#include <TenTurnPot.h>
#include <Map.h>
#include <Point.h>

<<<<<<< HEAD
#define TURRET_ANGLE_INCREMENT 10
#define TURRET_MAX_ANGLE 180
#define TURRET_MIN_ANGLE 0
#define TURRET_MIN_LIMIT -45
#define TURRET_MAX_LIMIT 405
#define TURRET_POWERLIMIT 60 //constrains to 30 - 150
=======
#define ANGLE_INCREMENT 15
#define MAX_ANGLE 375
#define MIN_ANGLE -15
>>>>>>> FETCH_HEAD

#define SERVO_ANGLE_INCREMENT 5
#define SERVO_MAX_ANGLE 45
#define SERVO_MIN_ANGLE 0
#define SERVO_MAX_ANGLE_OFFSET 149
#define SERVO_MIN_ANGLE_OFFSET 96

#define SERVO_DELAY 50 //milliseconds

#define IR_DATA_COLS ((TURRET_MAX_ANGLE - TURRET_MIN_ANGLE) / TURRET_ANGLE_INCREMENT)
#define IR_DATA_ROWS ((SERVO_MAX_ANGLE - SERVO_MIN_ANGLE) / SERVO_ANGLE_INCREMENT)

class Turret {

public:
	// Default constructor.
	Turret();

	// Attaches the servo
	void init();

	// Gets the angle of the turret (0..360]
	double getAngle();

	// Gets the distance to the closest obstacle.x
	// Returns -1 if no obstacle is detected
	double getDistance();

	// Gets the location of the closest obstacle relative to the robot.
	void getObstacleLocation();

	// Sets the angle of the turret to the given angle.
	// Returns true once it reaches that angle.
	boolean setTurretAngle(double angle);

	// Sets the angle of the IR sensor
	// 0 degrees is level to ground, and 45 degrees is 45 degrees below level
	void setServoAngle(double angle);

	// Scans 
	boolean scan();
	
	// Processes IR data and tries to find the flame location
	// Returns true once it completes
	boolean processIRData();

	void printObstacles();

	double obstacleXVals[((TURRET_MAX_ANGLE - TURRET_MIN_ANGLE) / TURRET_ANGLE_INCREMENT)];
	double obstacleYVals[((TURRET_MAX_ANGLE - TURRET_MIN_ANGLE) / TURRET_ANGLE_INCREMENT)];

private:
	TenTurnPot pot;
	Servo motor;
	Servo IRServo;
	double angle;
	double distance;
	Ultrasonic rangeSensor;
	int irData[IR_DATA_ROWS][IR_DATA_COLS];

	int scan_State;
	int scan_PosX;
	int scan_PosY;
	int scan_TurretAngle;
	int scan_ServoAngle;

	//initial values of
	//x = 36, 10 degree increments over 360 degrees
	//y = 9, 5 degree increments over 45 degrees
	// int scan_Value[36][9];
	int scan_Value[IR_DATA_ROWS][IR_DATA_COLS];
};

#endif