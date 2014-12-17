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

#define PRINTMAP // uncomment to print the map data to Serial

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

Point target(0, 0);
Point robotLocation(0, 0);
int state = 0;
double currentDist = 12;

double robotX = 0;
double robotY = 0;

void setup() {
	Serial.begin(115200);
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

	previousPoints->add(robotLocation);
	pinMode(START_BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
	// runStateMachine();
	// testMapping();
	testLightSensors();
}

void runStateMachine() {
	static int curAngle = 0;
	static Point prev;
	switch (currentState) {
	    case START:
	    	break;

	    case INCREMENT_TURRET:
	    	if(turret.setTurretAngle(curAngle)) {
	    		curAngle += TURRET_ANGLE_INCREMENT;
	    		currentState = CRUNCH_RANGE_DATA;
	    	}
	    	break;

	    case CRUNCH_RANGE_DATA:
	    	for(int i = 0; i < RANGE_SAMPLES_PER_INCREMENT; i++) {
	    		Point* obstacleLocation = turret.getObstacleLocation(i);
	    		fieldMap.set(robotX + obstacleLocation->x, robotY + obstacleLocation->y, true);
	    	}
	    	if(curAngle > TURRET_MAX_ANGLE) {
	    		curAngle = 0;
	    		currentState = CLEAN_MAP;
	    	} else {
	    		currentState = INCREMENT_TURRET;
	    	}
	    	break;

	    case CLEAN_MAP:
	    	#ifdef PRINTMAP fieldMap.printMap(); // takes a while, comment out if we don't need to see the initial map
	    	#endif
	    	fieldMap.filter();
	    	fieldMap.cleanUp();
	    	#ifdef PRINTMAP fieldMap.printMap(); // ditto as above
	    	#endif
	    	currentState = FIND_SETPOINT;
	    	break;

	    case FIND_SETPOINT:
	    	// target = getTarget();

	    	currentState = MOVE_TO_SETPOINT;
	    	break;

	    case MOVE_TO_SETPOINT:
	    	// move in direction returned by getNextDirection()
	    	// until we see the flame with continuous IR scanning
	    	// or until we're ~6" away from a wall.
	    	// When we stop, add current position to history so we
	    	// can backtrack to the beginning 
	    	break;

	    case TRIANGULATING:
	    	// Scan for accurate flame position, move some known distance, and scan again
	    	break;

	    case EXTINGUISHING:
	    	// Speed up the fan, run it for some set time, then stop it and go on to next state
	    	static boolean speedingUp = true;
	    	if(speedingUp && !fan.isAtMaxSpeed()) {
	    		fan.speedUp();
	    	}
	    	break;

	    case RETURNING: {
	    	Point target = previousPoints->pop();
	    	while(previousPoints->size() > 0) {
	    		if(moveToPoint(target.x, target.y)) {
	    			target = previousPoints->pop();
	    		}
	    	}
	    	currentState = COMPLETE;
	    	break;

	    }
	    case COMPLETE:
	    	// do something
	    	break;

	    default:
	    	// how did we get here?
	    	break;	      
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
byte getNextDirection() {
	Point target;
	byte direction   = 0;
	unsigned int max = 0;
	unsigned int sum1 = 0, sum2 = 0;

	for(int y = -FIELD_HEIGHT / 2; y < FIELD_HEIGHT / 2; y += 3) {
		for(int x = robotX; x < FIELD_WIDTH / 2; x += 3) {
			sum1 += fieldMap.get(x, y);
		}
		for(int x = -FIELD_WIDTH / 2; x < robotX; x += 3) {
			sum2 += fieldMap.get(x, y);
		}
	}
	if(sum1 > max) {
		max = sum1;
		direction = 1; // max is in positive X direction
	}
	if(sum2 > max) {
		max = sum2;
		direction = 3; // max is in negative X direction
	}

	sum1 = 0;
	sum2 = 0;
	for(int x = -FIELD_WIDTH / 2; x < FIELD_WIDTH / 2; x += 3) {
		for(int y = robotY; y < FIELD_HEIGHT / 2; y += 3) {
			sum1 += fieldMap.get(x, y);
		}
		for(int y = -FIELD_HEIGHT / 2; y < robotY; y += 3) {
			sum2 += fieldMap.get(x, y);
		}
	}
	if(sum1 > max) {
		max = sum1;
		direction = 2; // max is in positive Y direction
	}
	if(sum2 > max) {
		max = sum2;
		direction = 4; // max is in negative Y direction
	}

	return direction;
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
	    case 2: {
	    	// Drive the calculated distance and return true once it happens
	    	double distanceMoved = 0;
	    	if((distanceMoved = drive.driveDistance(moveToPointDist)) >= moveToPointDist) {
	    		robotX += distanceMoved * cos(toRad(drive.getAngle())); // getAngle should equal the calculated angle
	    		robotY += distanceMoved * sin(toRad(drive.getAngle()));
	    		moveToPointState = 0;
	    		return true;
	    	}
	    	break;
	    }
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



