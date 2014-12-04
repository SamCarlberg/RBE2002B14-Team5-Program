#include <Arduino.h>
#include <Constants.h>
#include <Turret.h>

Turret::Turret(): pot(1) {
}

void Turret::init() {
	motor.attach(TURRET_MOTOR_PIN);
}

double Turret::getAngle() {
	return int(pot.getAngle()) % 360;
}

double Turret::getDistance() {
	// return rangeFinder.read();
	return -1;
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

int angle = 0;
const int increment = 10; // degrees
const int MAX_ANGLE = 180;
const int MIN_ANGLE = 0;

void Turret::scan() {
	if(angle == MAX_ANGLE) {
		finishedScan = true;
		angle = MIN_ANGLE;
		return;
	} else {
		getObstacleLocation();
		getFlameLocation();
		angle += increment;
	}
	setAngle(angle);
}

boolean Turret::completedScan() {
	return finishedScan;
}

double Turret::getDistanceToFlame() {
	// return flamesensor.getDistance();
	return -1;
}