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

#define ANGLE_INCREMENT 5
#define MAX_ANGLE 180
#define MIN_ANGLE 0

#define SERVO_ANGLE_INCREMENT 5
#define SERVO_MAX_ANGLE 180
#define SERVO_MIN_ANGLE 0

#define IR_DATA_COLS ((MAX_ANGLE - MIN_ANGLE) / ANGLE_INCREMENT)
#define IR_DATA_ROWS ((SERVO_MAX_ANGLE - SERVO_MIN_ANGLE) / SERVO_ANGLE_INCREMENT)

class Turret {

public:
	// Default constructor.
	Turret();

	// Attaches the servo
	void init();

	// Gets the angle of the turret (0..360]
	double getAngle();

	// Gets the distance to the closest obstacle.
	// Returns -1 if no obstacle is detected
	double getDistance();

	// Gets the location of the closest obstacle relative to the robot.
	Point getObstacleLocation();

	// Sets the angle of the turret to the given angle.
	// Returns true once it reaches that angle.
	boolean setAngle(double angle);

	// Scans 
	boolean scan();
	
	// Processes IR data and tries to find the flame location
	// Returns true once it completes
	boolean processIRData();


	Point* obstacles[((MAX_ANGLE - MIN_ANGLE) / ANGLE_INCREMENT)];

private:
	TenTurnPot pot;
	Servo motor;
	double angle;
	double distance;
	Ultrasonic rangeSensor;
	byte irData[IR_DATA_ROWS][IR_DATA_COLS];
};

#endif