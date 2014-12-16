#include <Ultrasonic.h>
#include <Arduino.h>
#include <RunningMedian.h>

#define INCHES_PER_MICROSECOND (0.01351) // speed of sound at STP
#define NUM_SAMPLES 7
#define MAX_DISTANCE 36 // inches
#define DISTANCE_INFINITY 255
#define MAX_DELAY (MAX_DISTANCE / INCHES_PER_MICROSECOND)

Ultrasonic::Ultrasonic(int _trigger, int _echo): trigger(_trigger), echo(_echo) {
	pinMode(trigger, OUTPUT);
	pinMode(echo, INPUT);
	distance = 0;
}

double Ultrasonic::getRangeInches() {
	RunningMedian median(NUM_SAMPLES);
	int numOutliers = 0;
	for(int i = 0; i < NUM_SAMPLES; i++) {
		poll();
		if(distance >= MAX_DISTANCE) {
			if(numOutliers < NUM_SAMPLES / 2) {
				numOutliers++;
				distance = DISTANCE_INFINITY;
			} else {
				return DISTANCE_INFINITY;
			}
		}
		median.add(distance);
	}
	return median.getMedian(); // can change to median if outliers happen often enough
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