/*
    This class contains all the functionality of the swerve-style drive base on the robot.
 
 	Functions:
 
	 	init			called in setup, this attaches the servos to the appropriate pins
	 	rotatePods		rotates the pods to the given angle, where zero is parallel with the x-axis
	 	getAngle		gets the current angle of the pods
	 	drive			drives the robot at the given power (from -1 to 1 like in FRC)
	  	driveDistance	drives the robot at the given power for the given distance
 
 */

#ifndef SwerveDrive_h
#define SwerveDrive_h

#include <Servo.h>
#include <Pod.h>

class SwerveDrive {

public:
	// Default constructor
	SwerveDrive();

	// Rotates the pods to a certain angle
	void rotatePods(double angle);

	// Gets the current angle of the pods
	int getAngle();

	// Drives at the given power (-1 to 1 like in FRC)
	void drive(double power);

	// Drives the given distance at the given power
	void driveDistance(double power, double distInches);

	// Initializes servos. Call this in setup()
	void init();

private:
	int potPin;
	int frPin, flPin, rrPin, rlPin;
	int swerveMotorPin;
	Pod frontRight, frontLeft, rearRight, rearLeft;
	Servo swerveMotor;
};

#endif