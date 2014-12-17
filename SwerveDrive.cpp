#include <Servo.h>
#include <SwerveDrive.h>
#include <Pod.h>
#include <Constants.h>
#include <Wire.h>


// Constants for gyro filtering and adjustment
#define GYRO_POLL_PERIOD 20
#define GYRO_POS_OFFSET 95
#define GYRO_NEG_OFFSET 92
#define NUM_FILTER_SAMPLES (10)
#define kGyro (75.0/8500.0)

// Constants for driving straight
#define frShift (45 + 90 * 0)
#define rrShift (45 + 90 * 1)
#define rlShift (45 + 90 * 2)
#define flShift (45 + 90 * 3)
#define KpStraight (-10.0)

// Arcane. Resets the Arduino. Used to bully the gyro into initializing
void (*reset)() = 0;

SwerveDrive::SwerveDrive():
	frontRight(FR_MOTOR_PIN),
	frontLeft(FL_MOTOR_PIN),
	rearRight(RR_MOTOR_PIN),
	rearLeft(RL_MOTOR_PIN),
	pot(SWERVE_POT_PIN),
	xGyroFilter(NUM_FILTER_SAMPLES) {
	gyroAngle = 0;
	lastMillis = 0;
}

void SwerveDrive::init() {
	Wire.begin();
	if(!gyro.init()) {
		Serial.print("Could not initialize gyro! Resetting robot...\n\n\n");
		delay(1000);
		reset();
		while(1);
	}
	Serial.println("Gyro initialized");
	gyro.enableDefault();
	xGyroFilter.clear();
	swerveMotor.attach(CENTER, 1000, 2000);
	frontRight.init();
	frontLeft.init();
	rearRight.init();
	rearLeft.init();
}

int turningPower = 0;

boolean SwerveDrive::rotatePods(double angle, double tolerance) {
	double error = getAngle() - angle;
	if(abs(error) <= tolerance) {
		swerveMotor.write(90);
		drive(90);
		frontLeft.encoder.reset();
		frontRight.encoder.reset();
		rearLeft.encoder.reset();
		rearRight.encoder.reset();
		return true;
	}
	double power = constrain(error*4.0 + 90, 90 - 45, 90 + 45);
	double motorCorrection = (power - 90)/5.0 + 90;
	drive(motorCorrection);
	swerveMotor.write(power);
	return false;
}

void SwerveDrive::drivePods(double speed){
	swerveMotor.write(constrain(speed, 90 - 35, 90 + 35));
}

int SwerveDrive::getAngle() { 
	return pot.getAngle();
}

void SwerveDrive::drive(double power) {
	frontLeft.encoder.reset();
	frontRight.encoder.reset();
	rearLeft.encoder.reset();
	rearRight.encoder.reset();

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

// pod power = Kp * gyroAngle * sin(swerveAngle + 45 + 90 * podNum)
// FR = 1
// RR = 2
// RL = 3
// FL = 4
void SwerveDrive::driveStraight(double power) {
	double frPower = 180 - (power + KpStraight * pollGyro() * sin(toRad(getAngle() + frShift))); // inverted
	double flPower = power + KpStraight * pollGyro() * sin(toRad(getAngle() + flShift));
	double rrPower = 180 - (power + KpStraight * pollGyro() * sin(toRad(getAngle() + rrShift)));
	double rlPower = 180 - (power + KpStraight * pollGyro() * sin(toRad(getAngle() + rlShift)));

	frontRight.drive(constrain(frPower, 0, 180));
	frontLeft.drive(constrain(flPower, 0, 180));
	rearRight.drive(constrain(rrPower, 0, 180));
	rearLeft.drive(constrain(rlPower, 0, 180));
}

double distanceTravelled = 0;
double SwerveDrive::driveDistance(double distInches) {
	double avgDistance = (frontLeft.getDistance() + 
						  frontRight.getDistance() +
						  rearLeft.getDistance() +
						  rearRight.getDistance())
						 / 4.0;

	distanceTravelled = avgDistance;
	if(distanceTravelled >= distInches) { // have we driven that far?
		drive(90); // stop the motors

		// reset encoders
		frontLeft.encoder.reset();
		frontRight.encoder.reset();
		rearLeft.encoder.reset();
		rearRight.encoder.reset();
		double dist = distanceTravelled;
		distanceTravelled = 0; // reset distanceTravelled
		return dist;
	}
	driveStraight(135);
	return distanceTravelled;
}

double SwerveDrive::pollGyro() {
	long curTime = millis();
	if(curTime - lastMillis >= GYRO_POLL_PERIOD) {
		gyro.read();
		double offsetReading = gyro.g.x;

		// rate offset changes if the angle is positive or negative (very weird)
		if(gyroAngle > 0) {
			offsetReading -= GYRO_POS_OFFSET;
		} else {
			offsetReading -= GYRO_NEG_OFFSET;
		}

		xGyroFilter.add(offsetReading);

		gyroAngle += kGyro * ((xGyroFilter.getAverage() * GYRO_POLL_PERIOD)/ 1000.0);
		// Serial.println(gyroAngle);
		lastMillis = curTime;
	}
	return gyroAngle;
}









