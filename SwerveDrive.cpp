#include <Servo.h>
#include <SwerveDrive.h>
#include <Pod.h>
#include <Constants.h>

#define sign(x) (x < 0 ? -1 : x > 0 ? 1 : 0)

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

int turningPower = 0;

boolean SwerveDrive::rotatePods(int angle) {
	angle %= 360; // make sure that the given angle is in the range (0, 360]
	int error = getAngle() - angle;
	if(getAngle() == angle) {
		swerveMotor.write(90);
		return true;
	}
	double power = map(error, -359, 359, -1, 1);
	power = power * power * sign(power); // squared for lower speed at small angles
	swerveMotor.write(power);
	return false;
}

double swervePotMult = 360 / (1023.0 / 10); // 360 degrees per 102.3 counts on analogread

int SwerveDrive::getAngle() { 
	return int(analogRead(SWERVE_POT_PIN) * swervePotMult) % 360; // return a number in (0,360]
}

void SwerveDrive::drive(double power) {
	frontRight.drive(power);
	frontLeft.drive(power);
	rearRight.drive(power);
	rearLeft.drive(power);
}

boolean SwerveDrive::driveDistance(double power, double distInches) { 
}