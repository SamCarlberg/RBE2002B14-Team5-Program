#include <Pod.h>
#include <Encoder.h>

Pod::Pod(int _sPin, int _ePin):
	sPin(_sPin),
	encoder(_ePin, _ePin) {
}

void Pod::init() {
	servo.attach(sPin);
}

double Pod::getDistance() {
	return 0;
}

void Pod::drive(double power) {

}