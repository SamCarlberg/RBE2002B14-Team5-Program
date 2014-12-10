

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
	    	if(drive.driveDistance(6)) {
	    		testState = TURN1;
	    	}
	     	break;
	     case TURN1:
	     	if(drive.rotatePods(45)) {
	     		testState = DRIVE2;
	     	}
	     	break;
	     case DRIVE2:
	     	if(drive.driveDistance(6)) {
	     		testState = TURN2;
	     	}
	     	break;
	     case TURN2:
	     	if(drive.rotatePods(0)) {
	     		testState = DRIVE1;
	     	}
	     	break;
	    default:
	    	break;
	}
}

