/*

	Class representing a normal (non-quadrature) encoder.

*/

#ifndef SingleInterruptEncoder_h
#define SingleInterruptEncoder_h

class SingleInterruptEncoder {
public:
	// Constructor.
	SingleInterruptEncoder();

	// Tells the encoder there's been a tick.
	void update();

	// Gets the number of ticks registered. 
	uint32_t getTicks();

	// Gets the rotational speed of the encoder shaft.
	double getRPM();

	void reset() {
		rpm = 0;
		ticks = 0;
	}
private:
	double rpm;
	volatile uint32_t ticks;
};
#endif