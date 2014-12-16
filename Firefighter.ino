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


// One robot to rule them all,
//    One robot to find them
// One robot to bind them here
//    And of their sleep deprive them

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
#include <Point.h>
#include <LinkedList.h>

LiquidCrystal lcd(53, 51, 49, 47, 45, 43);
SwerveDrive drive;
Turret turret;
Map fieldMap;
Fan fan;
LinkedList<Point> *previousPoints = new LinkedList<Point>();

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

int numScans = 0;
int state = 0;
void loop() {
	// runStateMachine();
	// if(turret.scan(0, 360)){
	// 	lcd.print(turret.scan_XBar);
	// 	lcd.setCursor(0, 1);
	// 	lcd.print(turret.scan_YBar);

	// 	shouldKillFire = true;
	// 	turret.setServoAngle(-75);

		// while(!fan.isAtMaxSpeed()){
		// 	fan.speedUp();
		// }
		// delay(1000);
		// while(!fan.isStopped()){
		//     fan.slowDown();
		// }
		// delay(1000);

	// 	while(1){
			
	// 	}

	// 	// fan.speedUp();
	// 	// delay(10000);
	// 	// fan.slowDown();

	// }
	lcd.clear();
	lcd.print(state);

	switch (state) {
	    case 0:
			processObstacles();
			if(turret.scan()) {
				numScans++;
				if(numScans == 5) {
					fieldMap.printMap();
					numScans = 0;
					state++;
				}
			}
		    break;
	    case 1:
	    	Serial.println("Filtering first pass...");
	    	fieldMap.filter();
	    	fieldMap.printMap();
	    	Serial.println("Cleaning up...");
	    	fieldMap.cleanUp();
	    	fieldMap.printMap();
	    	state++;
	    	break;
	    default:
	    	break;
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
}

double distanceToPoint(double x, double y) {
	return sqrt(square(robotX - x) + square(robotY - y));
}

// Gets the next target point to drive to
Point getTarget() {
	Point nearestEndpoint(255, 255);
	double nearestDist = distanceToPoint(255, 255);
	for(int x = -FIELD_WIDTH / 2; x < FIELD_WIDTH / 2; x += 3) {
		for(int y = -FIELD_HEIGHT / 2; y <= FIELD_HEIGHT / 2; y += 3) {
			if(fieldMap.get(x, y) == 15) {
				double distToPoint = distanceToPoint(x, y);
				if(distToPoint < nearestDist && !visitedPoint(x, y)) {
					nearestEndpoint.x = x;
					nearestEndpoint.y = y;
					nearestDist = distToPoint;
				}
			}
		}
	}
	if(nearestEndpoint.x == 255 || nearestEndpoint.y == 255) {
		nearestEndpoint.x = 0; // make sure we don't try to move outside the field
		nearestEndpoint.y = 0;
		return nearestEndpoint;
	}
	// To move to offset point closest to robot:
	// setpoint is +x +y, offset point is -x -y
	// setpoint is +x -y, offset point is -x +y
	// setpoint is -x -y, offset point is +x +y
	// setpoint is -x +y, offset point is +x -y
	double distOffset = 6;
	nearestEndpoint.x -= sign(nearestEndpoint.x - robotX) * distOffset;
	nearestEndpoint.y -= sign(nearestEndpoint.y - robotY) * distOffset;
	previousPoints->add(nearestEndpoint);
	return nearestEndpoint;
}

boolean visitedPoint(double x, double y) {
	for(int i = 0; i < previousPoints->size(); i++) {
		if(previousPoints->get(i).x == x && previousPoints->get(i).y == y)
			return true;
	}
	return false;
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



