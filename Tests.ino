

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

int mappingState = 0;
const int numScans = 5;
void testMapping() {
	static int curAngle = 0;
	lcd.clear();
	lcd.print("State: ");
	lcd.setCursor(8, 0);
	lcd.print(mappingState);
	switch (mappingState) {
	    case 0: // increase the turret angle
	    	lcd.setCursor(0, 1);
	    	lcd.print(turret.getAngle());
	    	if(turret.setTurretAngle(curAngle)) {
	    		curAngle += TURRET_ANGLE_INCREMENT;
	    		mappingState = 1;
	    		lcd.clear();
	    		lcd.print("Moved " + String(curAngle));
	    	}
	    	break;
	    case 1: // read turret ultrasonic several times and store data in map
	    	lcd.clear();
	    	lcd.print("Reading ping");
	    	for(int i = 0; i < numScans; i++) {
	    		Point* obsLoc = turret.getObstacleLocation(i);
	    		fieldMap.set(robotX + obsLoc->x, robotY + obsLoc->y, true);
	    	}
	    	if(curAngle > TURRET_MAX_ANGLE) {
	    		curAngle = 0;
	    		mappingState = 2;
	    	} else {
	    		mappingState = 0;
	    	}
	    	break;
	    case 2: // add range data to map and apply filters
	    	fieldMap.printMap();
	    	fieldMap.filter();
	    	fieldMap.cleanUp();
	    	fieldMap.printMap();
	    	mappingState = 3;
	    	break;
	    case 3:
	    	if(turret.setTurretAngle(0)) {
	    		mappingState = 4;
	    	}
	    	break;
	    case 4: {
	    	if(moveToPoint(robotX + 36, robotY)) {
	    		mappingState = 5;
	    	}
	    	break;
	    }
	    case 5:
	    	fieldMap.printMap();
	    	fieldMap.filter();
	    	fieldMap.cleanUp();
	    	fieldMap.printMap();
	    	mappingState = 6;
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

void testLightSensors() {
	lcd.clear();
	lcd.print(analogRead(FRONT_LS_PIN));
	lcd.setCursor(5, 0);
	lcd.print(analogRead(RIGHT_LS_PIN));
	lcd.setCursor(0, 1);
	lcd.print(analogRead(REAR_LS_PIN));
	lcd.setCursor(5, 1);
	lcd.print(analogRead(LEFT_LS_PIN));
	delay(50);
}
