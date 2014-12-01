#include <Pod.h>
#include <Encoder.h>

#define ticksPerRev 90.0
#define inchesPerRev 6.0

Pod::Pod(int _sPin, int _ePin):
	sPin(_sPin),
	encoder(_ePin, _ePin) {
}

void Pod::init() {
	servo.attach(sPin, 1000, 2000);
}

double Pod::getDistance() {
	return (encoder.read() / ticksPerRev) * inchesPerRev;
}

void Pod::drive(double power) {
	servo.write(map(power, -1, 1, 0, 180));
}
