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

	// Gets the location of the flame relative to the robot.
	// Returns (-1, -1) if no 
	Point getFlameLocation();

	// Sets the angle of the turret to the given angle.
	// Returns true once it reaches that angle.
	boolean setAngle(int angle);

	//
	void scan();

	// Returns true if the turret has done a complete scan
	boolean completedScan();

	// Gets the distance to the flame.
	// Returns -1 if it can't find it
	double getDistanceToFlame();
private:
	TenTurnPot pot;
	Servo motor;
	double angle;
	double distance;
	boolean finishedScan;
	double distToFlame;
	// Ultrasonic rangeSensor;
};

#endif