#include <Arduino.h>
#include <Pod.h>
#include <SingleInterruptEncoder.h>
#include <Constants.h>

#define inchesPerRev 6.0
#define Kp 0.001 // TUNE THIS
#define Ki 0.000
#define Kd 0.01
#define INCHES_PER_TICK (((2.75) * PI) / 180.0) // = 0.011999

Pod::Pod(int _sPin):
	sPin(_sPin) {
	p = i = d = 0;
	lastError = -1;
	curSpeed = 90;
}

void Pod::init() {
	servo.attach(sPin, 1000, 2000);
}

double Pod::getDistance() {
	return encoder.getTicks() * INCHES_PER_TICK;
}

void Pod::drive(double power) {
	servo.write(power);
}

void Pod::driveRPM(double rpm) {
	double pid = this->calcPID(encoder.getRPM(), rpm);
	//if(millis() % 100 == 0) Serial.println((String)sPin + "\tCurrent RPM: " + (String)encoder.getRPM() + "\tPID: " + (String)pid);
	curSpeed = constrain(curSpeed + calcPID(encoder.getRPM(), rpm), 90, 180);
	servo.write(curSpeed);
}

double Pod::calcPID(double input, double setpoint) {
	double error = setpoint - input;
	if(lastError == -1) lastError = error;
	p = error;
	i += error;
	d = (error - lastError);
	lastError = error;
	return Kp * p + Ki * i + Kd * d;
}
