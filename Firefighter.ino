/*
	Main program for the firefighting robot. Each subsystem on the robot has it's own class,
	split into a .h and .cpp file. For example, the swerve-style drive base has
 		SwerveDrive.h
 		SwerveDrive.cpp
 
 	The .h file contains the variables and objects (such as servos, I/O pins, etc.) and function
	prototypes. The .cpp is only to implement those prototypes.
 
	Each subsystem class is responsible for controlling that subsystem based on input given from
	this main class.
 
 */

#include <SwerveDrive.h>
#include <Constants.h>
#include <Map.h>

const byte fieldWidth = 20;
const byte fieldHeight = 20;

SwerveDrive drive;
Map fieldMap(fieldWidth, fieldHeight);

void setup() {
	drive.init();
}

void loop() {
}

