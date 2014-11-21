#include <Servo.h>
#include <SwerveDrive.h>
#include <Pod.h>
#include <Constants.h>

SwerveDrive::SwerveDrive():
	frontRight(FR_MOTOR_PIN, FR_ENC_PIN),
	frontLeft(FL_MOTOR_PIN, FL_ENC_PIN),
	rearRight(RR_MOTOR_PIN, RR_ENC_PIN),
	rearLeft(RL_MOTOR_PIN, RL_ENC_PIN) {
}

void SwerveDrive::init() {
	swerveMotor.attach(CENTER);
	frontRight.init();
	frontLeft.init();
	rearRight.init();
	rearLeft.init();
}

void SwerveDrive::rotatePods(double angle) { 
}

int SwerveDrive::getAngle() { 
}

void SwerveDrive::drive(double power) {
}

void SwerveDrive::driveDistance(double power, double distInches) { 
}