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
#include <Line.h>

LiquidCrystal lcd(53, 51, 49, 47, 45, 43);
SwerveDrive drive;
Turret turret;
Map fieldMap;
Fan fan;

byte currentState = START;

double robotX = 0;
double robotY = 0;

boolean shouldKillFire = false;

void setup() {
	Serial.begin(9600);
	lcd.begin(16, 2);

	lcd.clear();
	lcd.print("Setup!");
	turret.init();
	fan.init();
	drive.init();

	lcd.clear();
	// lcd.print("Initialized");
	attachInterrupt(FR_ENC_PIN, updateEncoderFR, CHANGE);
	attachInterrupt(FL_ENC_PIN, updateEncoderFL, CHANGE);
	attachInterrupt(RR_ENC_PIN, updateEncoderRR, CHANGE);
	attachInterrupt(RL_ENC_PIN, updateEncoderRL, CHANGE);

	testMapSet();
}

void loop() {
	// runStateMachine();
	if(turret.scan(0, 360)){
		lcd.print(turret.scan_XBar);
		lcd.setCursor(0, 1);
		lcd.print(turret.scan_YBar);

		shouldKillFire = true;
		turret.setServoAngle(-75);

		while(!fan.isAtMaxSpeed()){
			fan.speedUp();
		}
		delay(300);
		while(!fan.isStopped()){
		    fan.slowDown();
		}

		while(1){
			
		}

	// 	// fan.speedUp();
	// 	// delay(10000);
	// 	// fan.slowDown();

	}

	// turret.setTurretAngle(252);
	// turret.setServoAngle(-75);

	// while(true){
	// 	fan.speedUp();
	// }

	// turret.setServoAngle(-75);
	// fan.speedUp();	
	// delay(10000);
	// fan.slowDown();

	// Serial.println(turret.getAngle());

	// int input = analogRead(0);
	// input = map(input, 200, 800, 0, 360);
	// turret.setTurretAngle(input);
	// Serial.println(input);

	// int input = analogRead(0);
	// input = map(input, 200, 800, 0, 180);
	// input = constrain(input, 0, 180);
	// turret.setServoAngle(input);
	// Serial.println(input);

	// testMapSet();
}


Point nextPoint(robotX, robotY);
void runStateMachine() {
	lcd.clear();
	switch(currentState) {
		case START: {
			lcd.clear();
			lcd.print("START");
			// Zero as many things as possible
			if(drive.rotatePods(0)) {
				currentState = SCANNING;
			}
			break;
		}
		case SCANNING: {
			lcd.clear();
			lcd.print("SCANNING");
			// scan for fire and obstacles
			if(turret.scan()) {
				currentState = CALCULATING;
			}		
			break;
		}
		case CALCULATING: {
			lcd.clear();
			lcd.print("CALCULATING");
			processObstacles();
			nextPoint = getTarget();
			currentState = MOVING;
			break;
		}
		case TURNING: { // Rotate pods
			lcd.clear();
			lcd.print("TURNING");
			break;
		}
		case MOVING: {
			lcd.clear();
			lcd.print("MOVING");
			lcd.setCursor(0, 1);
			lcd.print(String((int) nextPoint.x) + ", " + String((int) nextPoint.y));
			
			moveToPoint(nextPoint.x, nextPoint.y);

			break;
		}
		case EXTINGUISHING: {
			break;
		}
		case RETURNING: {
			/*
				either backtrack exactly
				or use A* or Dijkstra's algorithm to go back to the start

				if(atStart) currentState = COMPLETE;
				else goBackToStart();

			 */
			break;
		}
		case COMPLETE: {
			/*
				Display the x, y, z coordinates of the flame on the LCD.
			 */
			break;
		}
		default:
			return;
	}
}

// Adds obstacles to the map
void processObstacles() {
	for(int i = 0; i < ((TURRET_MAX_ANGLE - TURRET_MIN_ANGLE) / TURRET_ANGLE_INCREMENT); i++) {
		double turretX = turret.obstacleXVals[i];
		double turretY = turret.obstacleYVals[i];
		double realX = robotX + turretX;
		double realY = robotY + turretY;
		fieldMap.set(realX, realY, true); // flag the point as having an obstacle
		turret.obstacleXVals[i] = 0; // reset the arrays
		turret.obstacleYVals[i] = 0;
	}



	/*


		
		

	*/
}

// Gets the next target point to drive to
Point getTarget() {
	double x = robotX, y = robotY; // target point

	// TODO
	x += 6;
	y += 6;

	Point p(x, y);
	return p;
}


byte moveToPointState = 0;
double moveToPointDist, moveToPointAngle;

boolean moveToPoint(double x, double y) {
	switch (moveToPointState) {
		case 0:
			// Calculate distance and angle
			moveToPointDist = sqrt(square(robotX - x) + square(robotY - y));
			moveToPointAngle = toDeg(atan(y / x));
			moveToPointState = 1;
			break;
	    case 1:
	    	// Rotate pods to the correct angle
			if(drive.rotatePods(moveToPointAngle)) {
				moveToPointState = 2;
			}
	    	break;
	    case 2:
	    	// Drive the calculated distance and return true once it happens
	    	if(drive.driveDistance(moveToPointDist) >= moveToPointDist) {
	    		moveToPointState = 0;
	    		return true;
	    	}
	    	break;
	    default:
	    	moveToPointState = 0;
	    	break;
	}
	return false;
}


// Glue for attaching encoder interrupts
void updateEncoderFR() {
	Serial.println(FR_ENC_PIN);
	drive.frontRight.encoder.update();
}

void updateEncoderFL() {
	Serial.println(FL_ENC_PIN);
	drive.frontLeft.encoder.update();
}

void updateEncoderRR() {
	Serial.println(RR_ENC_PIN);
	drive.rearRight.encoder.update();	
}

void updateEncoderRL() {
	Serial.println(RL_ENC_PIN);
	drive.rearLeft.encoder.update();
}



