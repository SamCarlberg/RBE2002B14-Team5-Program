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
#include <Fan.h>
#include <L3G.h>
#include <RunningMedian.h>
#include <Ultrasonic.h>

LiquidCrystal lcd(53, 51, 49, 47, 45, 43);
SwerveDrive drive;
Turret turret;
Map fieldMap;
Fan fan;

byte currentState = START;

double robotX = 0;
double robotY = 0;

void setup() {
	Serial.begin(9600);
	lcd.begin(16, 2);
	turret.init();
	// fan.init();
	// drive.init();
	attachInterrupt(FR_ENC_PIN, updateEncoderFR, CHANGE);
	attachInterrupt(FL_ENC_PIN, updateEncoderFL, CHANGE);
	attachInterrupt(RR_ENC_PIN, updateEncoderRR, CHANGE);
	attachInterrupt(RL_ENC_PIN, updateEncoderRL, CHANGE);
}

boolean scanned = false;
void loop() {
// 	if(!scanned && turret.scan()) {
// 		scanned = true;
// 	}
// 	if(scanned) {
// 		if(turret.setAngle(0)) {
// 			while(1);
// 		}
// 	}
	if(turret.processIRData()) {
		Serial.print("\n\n");
		while(1);
	}
}

boolean turretScanned = false;
void runStateMachine() {
	double dist = 6; // inches
	switch(currentState) {
		case START:
			// Do stuff
			currentState = SCANNING;
			break;
		case SCANNING:
			if(!turretScanned && turret.scan()) {
				turretScanned = true;
			}
			if(turretScanned) {
				// Process obstacles
				for(int i = 0; i < ((MAX_ANGLE - MIN_ANGLE) / ANGLE_INCREMENT); i++) {
					Point obstacleLoc = *turret.obstacles[i];
					fieldMap.set(robotX + obstacleLoc.x,
								 robotY + obstacleLoc.y,
								 true);
				}
				// Process IR data
				if(turret.processIRData()) {
					currentState = EXTINGUISHING;
				} else {
					currentState = MOVING;
				}
			}			
			break;
		case MOVING:
			// Move some set distance, then go on to SCANNING
			if(drive.driveDistance(dist)) {
				robotX += sin(drive.getAngle()) * dist;
				robotY += cos(drive.getAngle()) * dist;
				currentState = SCANNING;
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
				or use A* or Dijkstra's algorithm to go back to the start

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