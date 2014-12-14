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

#include <Wire.h>
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

	lcd.clear();
	lcd.print("Setup!");
	turret.init();
	// fan.init();
	// drive.init();
	lcd.print("Initialized");
	attachInterrupt(FR_ENC_PIN, updateEncoderFR, CHANGE);
	attachInterrupt(FL_ENC_PIN, updateEncoderFL, CHANGE);
	attachInterrupt(RR_ENC_PIN, updateEncoderRR, CHANGE);
	attachInterrupt(RL_ENC_PIN, updateEncoderRL, CHANGE);
}

byte state = 0;
void loop() {
	switch (state) {
	    case 0:
		    if(turret.scan()) {
				for(int i = 0; i < ((MAX_ANGLE - MIN_ANGLE) / ANGLE_INCREMENT); i++) {
					double turretX = turret.obstacleXVals[i];
					double turretY = turret.obstacleYVals[i];
					double realX = robotX + turretX;
					double realY = robotY + turretY;
					fieldMap.set(realX, realY, 1); // set the point as probably having an obstacle
					// ... and then decrease the probability in each cell that the ultrasonic ping passed through
					turret.obstacleXVals[i] = 0; // reset the values
					turret.obstacleYVals[i] = 0;
				}
				fieldMap.printMap();
		    	state = 1;
		    }
		    break;
	    case 1:
	    	delay(5000); // simulate motion
	    	robotX += 6;
	    	state = 0;
	    	break;
	}
	lcd.clear();
	lcd.print(turret.getAngle());
	// switch (state) {
	//     case 0:
	//     	if(turret.setAngle(0)) state = 1;
	//     	break;
	//     case 1:
	//     	if(turret.setAngle(180)) state = 0;
	//     	break;
	// }
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
			// if(drive.driveDistance(dist)) {
			// 	robotX += cos(drive.getAngle()) * dist;
			// 	robotY += sin(drive.getAngle()) * dist;
			// 	currentState = SCANNING;
			// }
			delay(5000); // simulate the robot actually driving
			robotX += dist;
			currentState = SCANNING;
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