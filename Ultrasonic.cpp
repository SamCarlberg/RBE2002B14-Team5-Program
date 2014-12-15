#include <Ultrasonic.h>
#include <Arduino.h>
#include <RunningMedian.h>

#define INCHES_PER_MICROSECOND (0.01351) // speed of sound at STP
#define NUM_SAMPLES 3

Ultrasonic::Ultrasonic(int _trigger, int _echo): trigger(_trigger), echo(_echo) {
	pinMode(trigger, OUTPUT);
	pinMode(echo, INPUT);
	distance = 0;
}

double Ultrasonic::getRangeInches() {
	RunningMedian median(NUM_SAMPLES);
	for(int i = 0; i < NUM_SAMPLES; i++) {
		poll();
		median.add(distance);
	}
	return median.getAverage();
}

void Ultrasonic::poll() {
	digitalWrite(trigger, LOW);
	delayMicroseconds(2);
	digitalWrite(trigger, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigger, LOW);
	long duration = pulseIn(echo, HIGH); // returns micros
	delay(20);

	distance = duration * INCHES_PER_MICROSECOND / 2;
}