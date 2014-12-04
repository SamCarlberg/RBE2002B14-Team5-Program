#include <Servo.h>
#include <SwerveDrive.h>
#include <Pod.h>
#include <Constants.h>


SwerveDrive::SwerveDrive():
	frontRight(FR_MOTOR_PIN),
	frontLeft(FL_MOTOR_PIN),
	rearRight(RR_MOTOR_PIN),
	rearLeft(RL_MOTOR_PIN),
	pot(SWERVE_POT_PIN) {
}

void SwerveDrive::init() {
	swerveMotor.attach(CENTER);
	frontRight.init();
	frontLeft.init();
	rearRight.init();
	rearLeft.init();
}

int turningPower = 0;

boolean SwerveDrive::rotatePods(int angle) {
	angle %= 360; // make sure that the given angle is in the range (0, 360]
	int error = getAngle() - angle;
	if(getAngle() == angle) {
		swerveMotor.write(90);
		return true;
	}
	double power = map(error, -359, 359, -1, 1);
	power = sq(power); // squared for lower speed at small angles
	swerveMotor.write(power);
	return false;
}

int SwerveDrive::getAngle() { 
	return int(pot.getAngle()) % 360; // return a number in (0,360]
}

void SwerveDrive::drive(double power) {
	frontRight.drive(power);
	frontLeft.drive(power);
	rearRight.drive(power);
	rearLeft.drive(power);
}

double distanceTravelled = 0;
boolean SwerveDrive::driveDistance(double distInches) {
	if(distanceTravelled == distInches) { // have we driven that far?
		drive(0); // stop the motors
		distanceTravelled = 0; // reset distanceTravelled
		return true;
	}
	// simple P control
	double error = distInches - distanceTravelled;
	double power = map(error, 0, distInches, -1, 1);
	power = sq(power); // squared for lower speed at small distances
	drive(power);
	return false;
}