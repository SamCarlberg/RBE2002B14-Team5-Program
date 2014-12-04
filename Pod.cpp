#include <Arduino.h>
#include <Pod.h>
#include <SingleInterruptEncoder.h>
#include <Constants.h>

#define inchesPerRev 6.0

Pod::Pod(int _sPin):
	sPin(_sPin){
}

void Pod::init() {
	servo.attach(sPin, 1000, 2000);
}

double Pod::getDistance() {
	return encoder.getTicks() * DEGREES_PER_ENC_TICK * inchesPerRev;
}

void Pod::drive(double power) {
	servo.write(map(power, -1, 1, 0, 180));
}