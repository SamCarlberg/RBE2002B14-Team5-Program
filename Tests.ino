

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

enum TestStates {
	DRIVE1, TURN1,
	DRIVE2, TURN2,
	DRIVE3, TURN3,
	DRIVE4, TURN4
};

byte testState = DRIVE1;
void testDrivesWithTurns() {
	switch (testState) {
	    case DRIVE1:
	    	lcd.print("DRIVE1");
	    	if(drive.driveDistance(6)) {
	    		lcd.clear();
	    		testState = TURN1;
	    	}
	     	break;
	     case TURN1:
	    	lcd.print("TURN1");
	     	if(drive.rotatePods(180)) {
	     		testState = DRIVE2;
	     	}
	     	break;
	     case DRIVE2:
	    	lcd.print("DRIVE2");
	     	if(drive.driveDistance(6)) {
	     		testState = TURN2;
	     	}
	     	break;
	     case TURN2:
	    	lcd.print("TURN2");
	     	if(drive.rotatePods(0)) {
	     		testState = DRIVE1;
	     	}
	     	break;
	    default:
	    	break;
	}
}

void testUltrasonic() {
	Ultrasonic u(ULTRASONIC_TRIGGER_PIN, ULTRASONIC_ECHO_PIN);
	double dist = u.getRangeInches();
	lcd.clear();
	lcd.print(dist, DEC);
	Serial.print(dist); Serial.print('\n');
}

void testFan() {
	fan.speedUp();
}

boolean scanned = false;
int mappingState = 0;
int scanNumber = 0;
const int numScans = 5;
int numMoves = 0;
void testMapping() {
	switch(mappingState) {
		case 0:
			if(turret.scanUltrasonic()) {
				processObstacles();
				scanNumber++;
				if(scanNumber == numScans) {
					mappingState++;
					scanNumber = 0;
				}
			}
			break;
		case 1:
			fieldMap.filter();
			fieldMap.cleanUp();
			mappingState++;
			break;
		case 2:
			numMoves++;

			if(numMoves >= 3) {
				mappingState = -1;
				break;
			}

			Serial.print("Map update# "); Serial.println(numMoves);
			fieldMap.printMap();
			mappingState++;
			break;
		case 3:
			if(moveToPoint(robotX + 24, robotY)) {
				mappingState = 0;
			}
			break;
		default:
			break;
	}
}

void testMapSet() {

	for(int i = 0; i < 17; i++) {
		fieldMap.set(70,	 -10.6, true);
		fieldMap.set(73,	 -10.6, true);
		fieldMap.set(76,	 -10.6, true);
		fieldMap.set(79,	 -10.6, true);
	}

}

int wallFollowState = 0;
int currentDirection = 0;
int podAngle = 0;
void testRightWall() {
	switch(wallFollowState) {
		case 0:
			Serial.print("frontUltrasonic: ");Serial.println(frontUltrasonic.getRangeInches());
			Serial.print("rightUltrasonic: ");Serial.println(rightUltrasonic.getRangeInches());
			Serial.print("backUltrasonic: ");Serial.println(backUltrasonic.getRangeInches());
			Serial.print("leftUltrasonic: ");Serial.println(leftUltrasonic.getRangeInches());
			// if(frontUltrasonic.getRangeInches() <= 6) {
			// 	wallFollowState = 1;
			// } else if(rightUltrasonic.getRangeInches() <= 6) {
			// 	wallFollowState = 2;
			// } else if(backUltrasonic.getRangeInches() <= 6) {
			// 	wallFollowState = 3;
			// } else if(leftUltrasonic.getRangeInches() <= 6) {
			// 	wallFollowState = 4;
			// }
			break;
		case 1:
			if(currentDirection == 0) {
				drive.drive(90);
				wallFollowState = 5;
			} else {
				if(drive.rotatePods(180, 2)) {
					wallFollowState = 8;
				}
			}
			break;
		case 2:
			if(currentDirection == 1) {
				drive.drive(90);
				wallFollowState = 5;
			} else {
				if(drive.rotatePods(270, 2)){
					wallFollowState = 9;
				}
			}
			break;
		case 3:
			if(currentDirection == 2) {
				drive.drive(90);
				wallFollowState = 5;
			} else {
				if(drive.rotatePods(0, 2)) {
					wallFollowState = 10;
				}
			}
			break;
		case 4:
			if(currentDirection == 3) {
				drive.drive(90);
				wallFollowState = 5;
			} else {
				if(drive.rotatePods(90, 2)) {
					wallFollowState = 11;
				}
			}
			break;
		case 5:
			currentDirection = (currentDirection++) % 4;

			wallFollowState = 6;
			break;
		case 6:
			if(drive.rotatePods(currentDirection * 90), 2) {
				wallFollowState = 7;
			}
			break;
		case 7:
			drive.driveStraight(180);
			wallFollowState = 0;
			break;
		case 8:
			if(frontUltrasonic.getRangeInches() > 8) {
				wallFollowState = 6;
			} else {
				drive.driveStraight(180);
			}
			break;
		case 9:
			if(leftUltrasonic.getRangeInches() > 8) {
				wallFollowState = 6;
			} else {
				drive.driveStraight(180);
			}			
			break;
		case 10:
			if(backUltrasonic.getRangeInches() > 8) {
				wallFollowState = 6;
			} else {
				drive.driveStraight(180);
			}
			break;
		case 11:
			if(rightUltrasonic.getRangeInches() > 8) {
				wallFollowState = 6;
			} else {
				drive.driveStraight(180);
			}
			break;
		default:
			break;
	}
}