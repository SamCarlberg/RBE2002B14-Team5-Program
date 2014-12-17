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
#include <math.h>

LiquidCrystal lcd(53, 51, 49, 47, 45, 43);
SwerveDrive drive;
Turret turret;
Map fieldMap;
Fan fan;

byte currentState = START;

int state = 0;
double currentDist = 12;

double robotX = 0;
double robotY = 0;
int robotDirection = 0;
double robotCurrentDist = 0;
double quickScanAngle = 0;

double WALL_OFFSET = 4; //Distance from the robot to the closest wall, never to be exceeded

double Distance_North = 255;
double Distance_East = 255;
double Distance_South = 255;
double Distance_West = 255;

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

	pinMode(START_BUTTON_PIN, INPUT_PULLUP);

	// Serial.println(MAP_WIDTH * 8);
	// Serial.println(MAP_HEIGHT);
}

void loop() {

	// Serial.println(state);

	// switch (state) {
	// 	case 0:
	// 	  if(digitalRead(START_BUTTON_PIN) == 0){
	// 	  	state++;
	// 	  }
	// 	  break;
	//     case 1:
	//       if(drive.rotatePods(180)){
	//       	state++;
	//       }
	//       break;
	//     case 2:
	//       if(drive.driveDistance(currentDist) >= currentDist){
	//       	state++;
	//       }
	//       // drive.driveStraight(90);
	//       break;
	//     case 3:
	//       if(digitalRead(START_BUTTON_PIN) == 0){
	// 	  	state++;
	// 	  }
	//       break;
	//     case 4:
	//       if(drive.rotatePods(0)){
	//       	state++;
	//       }
	//       break;
	//     case 5:
	//       if(drive.driveDistance(currentDist) >= currentDist){
	//       	state = 0;
	//       	currentDist += 12;
	//       }
	//       break;

	//     default:
	//       state = 0;
	//       break;
	// }


	// runStateMachine();
	// double temp = drive.pollGyro();
	// Serial.println(temp);

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

		lcd.print("X:")
		lcd.setCursor(0, 1);
		lcd.print("Y:")
		lcd.setCursor(2, 0);
		lcd.print(candleXPos);
		lcd.setCursor(2, 1);
		lcd.print(candleYPos);
		lcd.setCursor(8, 0);
		lcd.print("Z:")
		lcd.setCursor(8, 2);
		lcd.print(candleZPos);



		turret.setServoAngle(-75);

		while(!fan.isAtMaxSpeed()){
			fan.speedUp();
		}
		delay(3000);
		while(!fan.isStopped()){
		    fan.slowDown();
		}

		while(1){
			
		}

		// fan.speedUp();
		// delay(10000);
		// fan.slowDown();

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

	// int input = analogRead(1);
	// Serial.print(analogRead(SWERVE_POT_PIN));
	// Serial.print(", ");
	// Serial.print(drive.getAngle());
	// Serial.print(", ");
	// input = constrain(map(input, 200, 800, 0, 360), 0, 360);
	// drive.rotatePods(input);
	// // input = constrain(map(input, 200, 800, 0, 180), 0, 180);
	// // drive.drivePods(input);
	// Serial.println(input);


	//This is the main state machine for wall following
	// switch (state) {

	// 	// Scan CCW for Candle
	// 	// If the scan was successful, the robot proceeds to advanced Candle detection
	// 	// If the scan failed, the robot proceeds to wall following
	// 	case 0:
	// 	  Serial.println("Case 0");
	// 	  lcd.clear();
	// 	  lcd.print("Case 0");
	// 	  if(digitalRead(START_BUTTON_PIN) == 0){
	// 	    state++;
	// 	  }
		  
	// 	  break;

	// 	case 1:
	// 	  Serial.println("Case 1");
	//       lcd.clear();
	// 	  lcd.print("Case 1");
	// 	  quickScanAngle = turret.quickScan();
	// 	    if(quickScanAngle){
	// 	  	  if(quickScanAngle >= 0){
	// 	  	    // If the quickScan found the candle, we proceed to
	// 	  	    // a more detailed scan for the candle
	// 	  	    state = 20; //CHANGE
	// 	  	  } else {
	// 	  	    //If the quickScan failed, we proceed to the next step
	// 	  	    state++;
	// 	   	  }
	// 	    }
	// 	    break;

	// 	// Get distances in the four cardinal directions
	//     case 2:
	//       Serial.println("Case 2");
	//       lcd.clear();
	// 	  lcd.print("Case 2");
	//       if(turret.setTurretAngle(270)){
	//       	Distance_West = turret.getDistance();
	//       	state++;
	//       }
	//       break;

	//     case 3:
	//       Serial.println("Case 3");
	//       lcd.clear();
	// 	  lcd.print("Case 3");
	//       if(turret.setTurretAngle(180)){
	//       	Distance_North = turret.getDistance();
	//       	state++;
	//       }
	//       break;

	//     case 4:
	//       Serial.println("Case 4");
	//       lcd.clear();
	// 	  lcd.print("Case 4");
	//       if(turret.setTurretAngle(90)){
	//       	Distance_East = turret.getDistance();
	//       	state++;
	//       }
	//       break;
	//     case 5:
	//       Serial.println("Case 5");
	//       lcd.clear();
	// 	  lcd.print("Case 5");
	//       if(turret.setTurretAngle(0)){
	//       	Distance_South = turret.getDistance();
	//       	state++;
	//       }
	//       break;

	//     // Set the robot's current direction, based on the four distances found
	//     // Goes away from the closest obstacle
	//     case 6:
	//       Serial.println("Case 6");
	//       lcd.clear();
	// 	  lcd.print("Case 6");
	//       if(Distance_North <= Distance_East && Distance_North <= Distance_South && Distance_North <= Distance_West){
	//       	robotDirection = 180;
	//       	robotCurrentDist = Distance_North;
	//       } else if (Distance_East <= Distance_South && Distance_East <= Distance_West){
	//       	robotDirection = 90;
	//       	robotCurrentDist = Distance_East;
	//       } else if (Distance_South <= Distance_West){
	//       	robotDirection = 0;
	//       	robotCurrentDist = Distance_South;
	//       } else {
	//       	robotDirection = 270;
	//       	robotCurrentDist = Distance_West;
	//       }
	//       state++;
	//       break;

	//     // Now that we know the direction of the closest wall, we will drive to it
	//     case 7:
	//       Serial.println("Case 7");
	//       lcd.clear();
	// 	  lcd.print("Case 7");
	//       if(drive.rotatePods(robotDirection)){
	// 	  	state++;
	// 	  }
	//       break;
	//     case 8:
	//       Serial.println("Case 8");
	//       lcd.clear();
	// 	  lcd.print("Case 8");
	// 	  delay(1000);
	// 	  lcd.clear();
	// 	  lcd.print(robotCurrentDist);
	// 	  delay(1000);
	//       if(drive.driveDistance(robotCurrentDist - WALL_OFFSET) >= robotCurrentDist - WALL_OFFSET) {
	//       	state++;
	//       }
	//       break;

	//     //Then we turn left by 90 Degree
	//     case 9:
	//       Serial.println("Case 9");
	//       lcd.clear();
	// 	  lcd.print("Case 9 ");
	//       robotDirection = (robotDirection + 90) % 360;
	//       state++;
	//       break;


	//     // Now we get into the repetative part of the code,
	//     // where we do the actual wall following

	//     //Here we do a quickScan for the candle
	//     case 10:
	//       Serial.println("Case 10");
	//       lcd.clear();
	// 	  lcd.print("Case 10");
	//       quickScanAngle = turret.quickScan();
	// 	  if(quickScanAngle){
	// 	  	if(quickScanAngle >= 0){
	// 	  	  // If the quickScan found the candle, we proceed to
	// 	  	  // a more detailed scan for the candle
	// 	  	  state = 20; //CHANGE
	// 	  	} else {
	// 	  	  //If the quickScan failed, we proceed to the next step
	// 	  	  state++;
	// 	  	}
	// 	  }
	//       break;

	//     // get distance to next wall
	//     case 11:
	//       Serial.println("Case 11");
	//       lcd.clear();
	// 	  lcd.print("Case 11");
	//       if(turret.setTurretAngle(robotDirection)){
	//       	robotCurrentDist = 100;
	//         robotCurrentDist = turret.getDistance();
	//       	state++;
	//       }
	//       break;

	//     // Now we turn the turret back to the robot's right, to wall follow
	//     case 12:
	//       Serial.println("Case 12");
	//       lcd.clear();
	// 	  lcd.print("Case 12");
	//       if(turret.setTurretAngle((robotDirection - 90) % 360)){
	//       	state++;
	//       }
	//       break;

	//     //Then we point the wheels in the right direction
	//     case 13:
	//       Serial.println("Case 13");
	//       lcd.clear();
	// 	  lcd.print("Case 13");
	//       if(drive.rotatePods(robotDirection)){
	// 	  	state++;
	// 	  }
	// 	  break;

	//     // Now we drive the robot towards the next wall until it sees a wall to it's right
	//     case 14:
	//       Serial.println("Case 14");
	//       lcd.clear();
	// 	  lcd.print("Case 14");
	//       if((drive.driveDistance(robotCurrentDist - WALL_OFFSET) >= robotCurrentDist - WALL_OFFSET)
	//       			|| /*check for cliff here*/ false || (turret.getDistance() < WALL_OFFSET*2)){

	//       	//If the robot "hits" the wall, it turns and goes back to quickscanning
	//       	if(drive.driveDistance(robotCurrentDist - WALL_OFFSET) >= robotCurrentDist - WALL_OFFSET){
	//       		state = 19; //CHANGE
	//       	}

	//       	// If the robot sees a cliff, it backs up, turns left, and returns to looking for a wall
	//       	if(/*check for cliff here*/ false){
	//       		state = 17; //Change
	//       	}

	//       	//If the robot sees a wall, it proceeds to the next step: looking for a gap
	//       	if(turret.getDistance() > WALL_OFFSET*2){
	//       		state++; //May need to be updated with a number
	//       	}

	//       	//Stop Robot
	//       	drive.drive(90);
	//       }
	//       break;

	//     // In this case, the robot saw a wall, and wants to continue driving until either it:
	//     // 		Sees another gap
	//     //		"Hits" the next wall
	//     //		Detects a cliff
	//     case 15:
	//       Serial.println("Case 15");
	//       lcd.clear();
	// 	  lcd.print("Case 15");
	//       if((drive.driveDistance(robotCurrentDist - WALL_OFFSET) >= robotCurrentDist - WALL_OFFSET)
	//       			|| /*check for cliff here*/ false || (turret.getDistance() > WALL_OFFSET*2)){

	//       	//If the robot "hits" the wall, it turns and goes back to quickscanning
	//       	if(drive.driveDistance(robotCurrentDist - WALL_OFFSET) >= robotCurrentDist - WALL_OFFSET){
	//       		state = 19; //CHANGE
	//       	}

	//       	// If the robot sees a cliff, it backs up, turns left, and returns to looking for a wall
	//       	if(/*check for cliff here*/ false){
	//       		state = 17; //Change
	//       	}

	//       	//If the robot sees a gap, it proceeds to the next step: driving forward and turning towards the gap
	//       	if(turret.getDistance() > WALL_OFFSET*2){
	//       		state++; //May need to be updated with a number
	//       	}

	//       	//Stop Robot
	//       	drive.drive(90);
	//       }
	//       break;

	//     //
	//     case 16:
	//       Serial.println("Case 16");
	//       lcd.clear();
	// 	  lcd.print("Case 16");
	//       if(drive.driveDistance(WALL_OFFSET) >= WALL_OFFSET){
	//       	robotDirection = (robotDirection - 90) % 360;
	//       	state = 10; //CHANGE
	//       }
	//       break;



	//     //These steps cover the case where the robot hits the cliff
	//     //First turn the pods backwards
	//     case 17:
	//       Serial.println("Case 17");
	//       lcd.clear();
	// 	  lcd.print("Case 17");
	//       if(drive.rotatePods((robotDirection + 180) % 360)){
	// 	  	state++;
	// 	  }
	//       break;

	//     //Then drive backwards and change the robot direction
	//     case 18:
	//       Serial.println("Case 18");
	//       lcd.clear();
	// 	  lcd.print("Case 18");
	//       if(drive.driveDistance(WALL_OFFSET) >= WALL_OFFSET){
	//       	robotDirection = (robotDirection + 90) % 360;
	//       	state = 10; //CHANGE
	//       }
	//       break;



	//     //These steps cover the case where the robot "hits" the wall
	//     case 19:
	//       Serial.println("Case 19");
	//       lcd.clear();
	// 	  lcd.print("Case 19");
	//       robotDirection = (robotDirection + 90) % 360;
	//       state = 10; //CHANGE
	//       break;



	//     //From here on out, we assume that the robot has found the candle and is simply putting it out
	//     case 20:
	//       Serial.println("Case 20");
	//       lcd.clear();
	// 	  lcd.print("Case 20");

	//       break;

	//     // 
	//     // case 5:

	//     //   break;

	//     // //
	//     // case 5:

	//     //   break;

	//     // //
	//     // case 5:

	//     //   break;

	//     // //
	//     // case 5:

	//     //   break;

	//     // //
	//     // case 5:

	//     //   break;

	//     // //
	//     // case 5:

	//     //   break;

	//     // //
	//     // case 5:

	//     //   break;

	//     // //
	    





	//     default:
	//       state = 0;
	//       break;
	// }

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
			lcd.print(nextPoint.x);
			lcd.setCursor(0, 7);
			lcd.print(nextPoint.y);
			
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

// Gets the next target point to drive to
Point getTarget() {
	double x = robotX, y = robotY; // target point

	// TODO

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
				moveToPointState = 1;
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



