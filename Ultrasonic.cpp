#include <Ultrasonic.h>
#include <Arduino.h>

#define POLL_PERIOD 100 // ms

Ultrasonic::Ultrasonic(int pin): analogPin(pin) {

}

double Ultrasonic::getRangeInches() {
	return getRangeMM() / 25.4;
}

double Ultrasonic::getRangeMM() {
	if(millis() - lastTime >= POLL_PERIOD) { //TODO change to use timer1
		lastTime = millis();
		poll();
	}
	return raw * 5;
}


void Ultrasonic::poll() {
	raw = analogRead(analogPin);
}