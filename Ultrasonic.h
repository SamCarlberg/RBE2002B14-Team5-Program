/*
	Class representing a Maxbotix ultrasonic sensor. This class uses the analog output of
	the sensor to calculate the range.
 */

#ifndef Ultrasonic_h
#define Ultrasonic_h

class Ultrasonic {
	public:
		// Constructor. Takes the analog pin to read from
		Ultrasonic(int);

		// Gets the range, in inches
		double getRangeInches();

		// Gets the range, in mm
		double getRangeMM();
	private:
		// Polls the ultrasonic for new data.
		// The sensor updates every 100 ms.
		void poll();
		int raw;
		int analogPin;
		long lastTime;
};

#endif