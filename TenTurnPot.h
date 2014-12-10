/*
	Class representing a ten-turn potentiometer.
	This is a very simple class, so everything is in a header file.
 */

#ifndef TenTurnPot_h
#define TenTurnPot_h

#include <Arduino.h>

#define ZERO 670
#define MINUS_90 (ZERO + 128) // 450/3600 * 1023 = 127.875

class TenTurnPot {
public:
	// Constructor.
	TenTurnPot(int _pin): pin(_pin){}

	// Gets the angle of the pot in degrees. 
	double getAngle() {
		return map(analogRead(pin), MINUS_90, ZERO, -90, 0);
	}

private:
	int pin;
};

#endif