

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
