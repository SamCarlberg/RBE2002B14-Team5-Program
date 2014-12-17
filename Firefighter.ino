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

// #define PRINTMAP // uncomment to print the map data to Serial

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
	// while(1){
	// 	lcd.clear();
	//     lcd.print(drive.getAngle());// statement
	//  if(drive.rotatePods(0)) {
	//  	//break;
	//  }
	// }
}

void loop() {
	// runStateMachine();
	testMapping();
	// drive.driveStraight(135);
	// lcd.clear();
	// lcd.print(drive.getAngle());
}

void runStateMachine() {
	static int curAngle = 0;
	static int maxIRValue, maxIRAngle;
	static Point prev;
	static byte dir;

	lcd.clear();
	lcd.print(currentState);

	switch (currentState) {
	    case START:
	    	if(digitalRead(12) == 0) {
	    		currentState = INCREMENT_TURRET;
	    	}
	    	break;

	    case INCREMENT_TURRET:
	    	if(turret.setTurretAngle(curAngle)) {
	    		curAngle += TURRET_ANGLE_INCREMENT;
	    		currentState = CRUNCH_RANGE_DATA;
	    	}
	    	break;

	    case CRUNCH_RANGE_DATA: {
	    	for(int i = 0; i < RANGE_SAMPLES_PER_INCREMENT; i++) {
	    		Point* obstacleLocation = turret.getObstacleLocation(i);
	    		fieldMap.set(robotX + obstacleLocation->x, robotY + obstacleLocation->y, true);
	    	}
	    	if(curAngle > TURRET_MAX_ANGLE) {
	    		curAngle = 0;
	    		if(maxIRValue >= IR_THRESHOLD) {
	    			currentState = TRIANGULATING;
	    		} else {
	    			currentState = CLEAN_MAP;
	    		}
	    	} else {
	    		currentState = INCREMENT_TURRET;
	    	}

	    	int currentIRValue = (1024 - 18) - analogRead(IR_SENSOR_PIN);

			if(currentIRValue > maxIRValue && currentIRValue > IR_THRESHOLD){
				maxIRValue = currentIRValue;
				maxIRAngle = turret.getAngle();
			}
	    	break;
	    }
	    case CLEAN_MAP:
	    	// fieldMap.printMap(); // takes a while, comment out if we don't need to see the initial map
	    	fieldMap.filter();
	    	fieldMap.cleanUp();
	    	// fieldMap.printMap(); // ditto as above
	    	currentState = FIND_SETPOINT;
	    	break;

	    case FIND_SETPOINT: {
	    	// target = getTarget();
	    	dir = getNextDirection();
	    	lcd.setCursor(0, 1);
	    	lcd.print(dir);
	    	switch (dir) {
	    		case 1:
	    			target.x = FIELD_WIDTH / 2;
	    			target.y = robotY;
	    			break;
	    		case 2:
	    			target.x = robotX;
	    			target.y = FIELD_HEIGHT / 2;
	    			break;
	    		case 3:
	    			target.x = -FIELD_WIDTH / 2;
	    			target.y = robotY;
	    			break;
	    		case 4:
	    			target.x = robotX;
	    			target.y = -FIELD_HEIGHT / 2;
	    			break;
	    	}
	    	while(1) {
	    		if(digitalRead(12) == 0) {
	    			break;
	    		}
	    	}
	    	currentState = AIM_TURRET;
	    	break;
	    }

	    case AIM_TURRET:
	    	if(turret.setTurretAngle((dir - 1) * 90)) {
	    		currentState = MOVE_TO_SETPOINT;
	    	}
	    	break;

	    case MOVE_TO_SETPOINT:
	    	// move in direction returned by getNextDirection()
	    	// until we see the flame with continuous IR scanning
	    	// or until we're ~6" away from a wall.
	    	// When we stop, add current position to history so we
	    	// can backtrack to the beginning
	    	if(turret.getDistance() <= 6) {
	    		drive.drive(90);
	    		currentState = INCREMENT_TURRET;
	    	} else if(moveToPoint(target.x, target.y)) {

	    	}
	    	break;

	    case TRIANGULATING:
	    	if(turret.scan(0, 360)){
				double candleXPos = 0;
				double candleYPos = 0;
				double candleZPos = 0;
				double candleDist = 0;

				// lcd.print(turret.scan_Sum);

				// candleDist = (-1050.0) * log(turret.scan_Sum) + 2475.0;
				candleDist = 12 * exp((turret.scan_Sum - 2475.0)/(-1050.0));

				// lcd.print(candleDist);

				candleXPos = cos(toRad(turret.scan_XBar - 90)) * candleDist;
				candleYPos = sin(toRad(turret.scan_XBar - 90)) * candleDist;
				candleZPos = 10.75 - sin(toRad(turret.scan_YBar)) * candleDist;

				lcd.print("X:");
				lcd.setCursor(0, 1);
				lcd.print("Y:");
				lcd.setCursor(2, 0);
				lcd.print(candleXPos);
				lcd.setCursor(2, 1);
				lcd.print(candleYPos);
				lcd.setCursor(8, 0);
				lcd.print("Z:");
				lcd.setCursor(8, 2);
				lcd.print(candleZPos);


				turret.setServoAngle(-75);
				currentState = EXTINGUISHING;
			}
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
	int maxDist = 0, tempDist = 0;
	int curY = (int)(robotY / 3) + 32;
	int curX = (int)(robotX / 6) + 16;
	int curNib = (int)(robotX / 3) % 2;
	boolean brk = false;

	for(int x = curX; x < MAP_WIDTH; x++) {
		for(int i = 0; i < 2; i++) {
			if(fieldMap.getValue(x, curY, i) < 14) {
				tempDist++;
			} else {
				brk = true;
				break;
			}
		}
		if(brk) {
			brk = false;
			break;
		}

	}
	Serial.println(tempDist);
	if(tempDist > maxDist) {
		maxDist = tempDist;
		direction = 1;
	}



	tempDist = 0;
	for(int x = curX; x > 0; x--) {
		for(int i = 0; i < 2; i++) {
			if(fieldMap.getValue(x, curY, i) < 14) {
				tempDist++;
			} else {
				brk = true;
				break;
			}
		}
		if(brk) {
			break;
		}
	}
	Serial.println(tempDist);
	if(tempDist > maxDist) {
		maxDist = tempDist;
		direction = 3;
	}


	tempDist = 0;
	for(int y = curY; y < MAP_HEIGHT; y ++) {
		if(fieldMap.getValue(curX, y, curNib) < 14) {
			tempDist++;
		} else {
			break;
		}
	}
	Serial.println(tempDist);
	if(tempDist > maxDist) {
		maxDist = tempDist;
		direction = 2;
	}


	tempDist = 0;
	for(int y = curY; y > 0; y--) {
		if(fieldMap.getValue(curX, y, curNib) < 14) {
			tempDist++;
		} else {
			break;
		}
	}
	Serial.println(tempDist);
	if(tempDist > maxDist) {
		maxDist = tempDist;
		direction = 4;
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
	static byte moveToPointState = 0;
	static double moveToPointDist, moveToPointAngle;
	switch (moveToPointState) {
		case 0:
			// Calculate distance and angle
			moveToPointDist = sqrt(square(robotX - x) + square(robotY - y));
			moveToPointAngle = toDeg(atan(y / (x == 0 ? 1 : 0)));
			moveToPointState = 1;
			break;
	    case 1:
			if(drive.rotatePods(moveToPointAngle)) {
				moveToPointState = 2;
			}
			lcd.setCursor(0, 1);
			lcd.print(drive.getAngle());
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



