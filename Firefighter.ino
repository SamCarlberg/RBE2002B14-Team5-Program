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
#include <Turret.h>
#include <Constants.h>
#include <Map.h>
#include <LiquidCrystal.h> 
#include <Wire.h>
#include <L3G.h>
#include <RunningMedian.h>

#define NUM_FILTER_SAMPLES 21
#define GYRO_POLL_PERIOD 20 // ms

const byte fieldWidth  = 20;
const byte fieldHeight = 20;

// LiquidCrystal lcd(0,0,0,0,0,0);
SwerveDrive drive;
Turret turret;
Map fieldMap(fieldWidth, fieldHeight);
L3G gyro;

RunningMedian xFilter(NUM_FILTER_SAMPLES); // we only care about the gyro on the X axis

byte currentState = START;

double robotX = 0;
double robotY = 0;

void setup() {
	Serial.begin(9600);
	drive.init();
	// turret.init();
	attachInterrupt(FR_ENC_PIN, updateEncoderFR, CHANGE);
	attachInterrupt(FL_ENC_PIN, updateEncoderFL, CHANGE);
	attachInterrupt(RR_ENC_PIN, updateEncoderRR, CHANGE);
	attachInterrupt(RL_ENC_PIN, updateEncoderRL, CHANGE);
	xFilter.clear();
}

boolean rotated = false, drove = false;
void loop() {
	if(!rotated && drive.rotatePods(0, 2)) {
		// Serial.println("Done!");
		// Serial.println(drive.getAngle());
		// Serial.println(analogRead(A11));
		rotated = true;
	}
	if(rotated && !drove && drive.driveDistance(24)) {
		drove = true;
	}
}

// Drives one pod
void testPodRPM(double rpm) {
	drive.frontLeft.driveRPM(rpm);
}

void testDrivePower(double power) {
	drive.drive(power);
}

// Drives all pods
void testDriveRPM(double rpm) {
	drive.driveRPM(rpm);
}

// Moves the pods to face forwards
void testSwerve(int angle) {
	while(drive.rotatePods(angle));
}

void testGyro() {
	pollGyro();
}

long lastMillis = 0;
float pollGyro() {
	if(millis() - lastMillis >= GYRO_POLL_PERIOD) {
		lastMillis = millis();
		gyro.read();
		xFilter.add(gyro.g.x);
	}
	return xFilter.getMedian();
}

void runStateMachine() {
	double x = 0, y = 0;
	double dist = 6; // inches
	switch(currentState) {
		case START:
			// Do stuff
			break;
		case MOVING:
			// Move some set distance, then go on to SCANNING
			if(drive.driveDistance(dist)) {
				robotX += sin(drive.getAngle()) * dist;
				robotY += cos(drive.getAngle()) * dist;
				currentState = SCANNING;
			}
			break;
		case SCANNING:
			// Move the turret and scan for obstacles and the candle
			// if(turret.setAngle(10) && !turret.completedScan()) turret.scan();
			// else break;

			fieldMap.set(robotX + turret.getObstacleLocation().x,
						 robotY + turret.getObstacleLocation().y,
						 true); // set the point at (x, y) to have an obstacle

			if(!isNullPoint(turret.getFlameLocation())) {
				currentState = EXTINGUISHING;
			} else {
				currentState = MOVING;
			}
			
			break;
		case EXTINGUISHING:
			/*
				if(flameVisible) turret.extinguishFlame();
				else currentState = RETURNING;

			 */
			break;
		case RETURNING:
			/*
				either backtrack exactly
				or use A* or Djikstra's algorithm to go back to the start

				if(atStart) currentState = COMPLETE;
				else goBackToStart();

			 */
			break;
		case COMPLETE:
			/*
				Display the x, y, z coordinates of the flame on the LCD.
			 */
			break;
		default:
			return;
	}
}

// Glue for attaching interrupts
void updateEncoderFR() {
	drive.frontRight.encoder.update();
}

void updateEncoderFL() {
	drive.frontLeft.encoder.update();
}

void updateEncoderRR() {
	drive.rearRight.encoder.update();	
}

void updateEncoderRL() {
	drive.rearLeft.encoder.update();
}