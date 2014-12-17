/*
	Class representing a ten-turn potentiometer.
	This is a very simple class, so everything is in a header file.
 */

#ifndef TenTurnPot_h
#define TenTurnPot_h

#include <Arduino.h>

#define ZERO 720
#define MINUS_90 837
#define ZERO_DEGREES 608
#define THREE_SIXTY_DEGREE 205
#define NINETY_DEGREES 485

class TenTurnPot {
public:
	// Constructor.
	TenTurnPot(int _pin, int _minus90 = MINUS_90, int _zero = ZERO):
		pin(_pin), 
		minus90(_minus90),
		zero(_zero) {
	}

	// Gets the angle of the pot in degrees. 
	double getAngle() {
		// return map(analogRead(pin), minus90, zero, -90, 0);
		return map(analogRead(pin), ZERO_DEGREES, NINETY_DEGREES, 0, 90);
	}

private:
	int pin;
	int minus90, zero;
};

#endif