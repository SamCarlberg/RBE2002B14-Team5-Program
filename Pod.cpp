#include <Pod.h>
#include <Encoder.h>

#define ticksPerRev 90
#define inchesPerRev 6

Pod::Pod(int _sPin, int _ePin):
	sPin(_sPin),
	encoder(_ePin, _ePin) {
}

void Pod::init() {
	servo.attach(sPin);
}

double Pod::getDistance() {
	return (encoder.read() / ticksPerRev) * inchesPerRev;
}

void Pod::drive(double power) {
	
}
