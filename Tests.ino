

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
void testMapping() {
	if(!scanned && turret.scan()) {
		scanned = true;
	}
	if(scanned) {
		if(turret.setTurretAngle(0)) {
			for(int i = 0; i < ((TURRET_MAX_ANGLE - TURRET_MIN_ANGLE) / TURRET_ANGLE_INCREMENT); i++) {
				double x = turret.obstacleXVals[i];
				double y = turret.obstacleYVals[i];
				double realX = robotX + x;
				double realY = robotY + y;
				fieldMap.set(realX, realY, true);
				turret.obstacleXVals[i] = 0; // reset the values
				turret.obstacleYVals[i] = 0;
			}
			fieldMap.printMap();
			while(1);
		}
	}
}
