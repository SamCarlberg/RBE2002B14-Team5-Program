/*
	Class representing a Vex ultrasonic sensor.
	This uses two digital pins (one trigger and one echo).
 */

#ifndef Ultrasonic_h
#define Ultrasonic_h

class Ultrasonic {
	public:
		// Constructor. Parameters are trigger and echo pins
		Ultrasonic(int, int);

		// Gets the range, in inches
		double getRangeInches();
		
	private:
		// Polls the sensor to get a new reading
		void poll();

		// Digital trigger and echo pins
		int trigger, echo;

		// Distance in inches to nearest obstacle
		double distance;
};

#endif