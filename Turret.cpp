#include <Arduino.h>
#include <Constants.h>
#include <Turret.h>

Turret::Turret():
	pot(1),
	rangeSensor(TURRET_ULTRA_PIN){
}

void Turret::init() {
	motor.attach(TURRET_MOTOR_PIN);
}

double Turret::getAngle() {
	return pot.getAngle();
}

double Turret::getDistance() {
	return rangeSensor.getRangeInches();
}

Point Turret::getObstacleLocation() {
	double dist = getDistance();
	double angle = getAngle();
	Point p(dist * cos(angle), dist * sin(angle));
	return p;
}

Point Turret::getFlameLocation() {
	double dist = getDistanceToFlame();
	if(dist == -1) {
		Point p(-1, -1);
		return p;
	}
	double angle = getAngle();
	Point p(dist * cos(angle), dist * sin(angle));
	return p;
}

boolean Turret::setAngle(int angle) {
	return false;
}

int curAngle = 0;
const int increment = 10; // degrees
const int MAX_ANGLE = 180;
const int MIN_ANGLE = 0;

void Turret::scan() {
	if(curAngle == MAX_ANGLE) {
		finishedScan = true;
		angle = MIN_ANGLE;
		return;
	} else {
		getObstacleLocation();
		getFlameLocation();
		angle += increment;
	}
	setAngle(curAngle);
}

boolean Turret::completedScan() {
	return finishedScan;
}

double Turret::getDistanceToFlame() {
	// return flamesensor.getDistance();
	return -1;
}