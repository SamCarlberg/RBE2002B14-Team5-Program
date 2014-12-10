#include <Arduino.h>
#include <SingleInterruptEncoder.h>
#include <Constants.h>

#define ticksPerRPMUpdate 4
#define RPM_TIMEOUT 100

// Helper function to calculate RPM after some numbeer of ticks have passed
double calcRPM(long dt) {
	return (ticksPerRPMUpdate / (dt / 60000.0)) * DEGREES_PER_ENC_TICK / 360.0;
}

SingleInterruptEncoder::SingleInterruptEncoder(): ticks(0), rpm(0) {}

unsigned long lastTime = 0;

void SingleInterruptEncoder::update() {
	ticks++;
	if(ticks % ticksPerRPMUpdate == 0) {
		unsigned long time = millis();
		rpm = calcRPM(time - lastTime);
		lastTime = time;
	}
}

uint32_t SingleInterruptEncoder::getTicks() {
	return ticks;
}

double SingleInterruptEncoder::getRPM() {
	if(millis() - lastTime >= RPM_TIMEOUT) rpm = 0;
	return rpm;
}