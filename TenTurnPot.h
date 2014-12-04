/*
	Class representing a ten-turn potentiometer.
	This is a very simple class, so everything is in a header file.
 */

#ifndef TenTurnPot_h
#define TenTurnPot_h

#include <Arduino.h>

class TenTurnPot {
public:
	// Constructor.
	TenTurnPot(int _pin): pin(_pin){}

	// Gets the angle of the pot in degreees (zero through 3600)
	double getAngle() {
		return map(analogRead(pin), 0, 1023, 0, 3600);
	}

private:
	int pin;
};

#endif