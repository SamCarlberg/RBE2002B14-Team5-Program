/*
    This class contains all the functionality of the swerve-style drive base on the robot.
 
 	Functions:
 
	 	init			called in setup, this attaches the servos to the appropriate pins
	 	rotatePods		rotates the pods to the given angle, where zero is parallel with the x-axis
	 	getAngle		gets the current angle of the pods
	 	drive			drives the robot at the given power (from -1 to 1 like in FRC)
	  	driveDistance	drives the robot the given distance
 
 */

#ifndef SwerveDrive_h
#define SwerveDrive_h

#include <Servo.h>
#include <Pod.h>
#include <TenTurnPot.h>
#include <L3G.h>
#include <RunningMedian.h>

class SwerveDrive {

public:
	// Default constructor
	SwerveDrive();

	// Rotates the pods to a certain angle relative to the drive base
	// Returns true if the pods have rotated all the way, otherwise returns false
	// Tolerance is in degrees and defaults to 5
	boolean rotatePods(int angle, int tolerance = 5);

	// Gets the current angle of the pods relative to the drive base
	int getAngle();

	// Drives at the given power (-1 to 1 like in FRC)
	void drive(double power);

	void driveRPM(double rpm);

	// Drives the given distance
	// Returns true once the robot has driven that distance
	boolean driveDistance(double distInches);

	// Initializes servos. Call this in setup()
	void init();

	// Drives the robot in a straight line
	void driveStraight(double power);

	// Gets the current gyro angle
	double pollGyro();
	
	Pod frontRight, frontLeft, rearRight, rearLeft;
	Servo swerveMotor;

private:
	L3G gyro;
	RunningMedian xGyroFilter;
	TenTurnPot pot;
	int frPin, flPin, rrPin, rlPin;
	int swerveMotorPin;
	double gyroAngle;
	long lastMillis;
};

#endif