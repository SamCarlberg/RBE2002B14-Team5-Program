#include <Arduino.h>
#include <SingleInterruptEncoder.h>
#include <Constants.h>

#define ticksPerRPMUpdate 10

#define calcRPM(dt) ((((ticksPerRPMUpdate / (dt / 1000.0)) * DEGREES_PER_ENC_TICK) / 360.0) / 60.0)

SingleInterruptEncoder::SingleInterruptEncoder(): ticks(0), rpm(0) {}

unsigned long lastTime = 0;

void SingleInterruptEncoder::update() {
	ticks++;
	if(ticks % ticksPerRPMUpdate == 0) {
		unsigned long time = millis();
		unsigned int dt = time - lastTime;
		lastTime = time;
		rpm = calcRPM(dt);
	}
}

uint32_t SingleInterruptEncoder::getTicks() {
	return ticks;
}

double SingleInterruptEncoder::getRPM() {
	return rpm;
}