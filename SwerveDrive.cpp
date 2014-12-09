#include <Servo.h>
#include <SwerveDrive.h>
#include <Pod.h>
#include <Constants.h>

#define Kp (4.0)

SwerveDrive::SwerveDrive():
	frontRight(FR_MOTOR_PIN),
	frontLeft(FL_MOTOR_PIN),
	rearRight(RR_MOTOR_PIN),
	rearLeft(RL_MOTOR_PIN),
	pot(SWERVE_POT_PIN) {
}

void SwerveDrive::init() {
	swerveMotor.attach(CENTER, 1000, 2000);
	frontRight.init();
	frontLeft.init();
	rearRight.init();
	rearLeft.init();
}

int turningPower = 0;

boolean SwerveDrive::rotatePods(int angle, int tolerance) {
	int error = angle - getAngle();
	Serial.println("Error = " + String(error));
	if(abs(error) <= tolerance) {
		swerveMotor.write(90);
		drive(90);
		return true;
	}
	double power = constrain(error*Kp + 90, 0, 180);
	double motorCorrection = (constrain(error*Kp + 90, 0, 180) - 90)/5.0 + 90;
	drive(motorCorrection);
	swerveMotor.write(power);
	return false;
}

int SwerveDrive::getAngle() { 
	return pot.getAngle();
}

void SwerveDrive::drive(double power) {
	frontRight.drive(power);
	frontLeft.drive(power);
	rearRight.drive(power);
	rearLeft.drive(power);
}

void SwerveDrive::driveRPM(double rpm) {
	frontRight.driveRPM(rpm);
	frontLeft.driveRPM(rpm);
	rearRight.driveRPM(rpm);
	rearLeft.driveRPM(rpm);
}

double distanceTravelled = 0;
boolean SwerveDrive::driveDistance(double distInches) {
	Serial.println("Drove: " + String(distanceTravelled));
	if(distanceTravelled >= distInches) { // have we driven that far?
		drive(90); // stop the motors
		frontLeft.encoder.reset();
		frontRight.encoder.reset();
		rearLeft.encoder.reset();
		rearRight.encoder.reset();
		distanceTravelled = 0; // reset distanceTravelled
		return true;
	}
	double avgDistance = (frontLeft.getDistance() + frontRight.getDistance() + rearLeft.getDistance() + rearRight.getDistance()) / 4.0;
	distanceTravelled = avgDistance;
	drive(50);
	return false;
}