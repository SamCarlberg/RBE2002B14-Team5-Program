#ifndef Pod_h
#define Pod_h

#include <Servo.h>
#include <Encoder.h>

class Pod {
	public:
		// Constructor.
		Pod(int motorPin, int encoderPin);

		// Attaches the servo.
		void init();

		// Gets the distance travelled (in inches) by the pod
		double getDistance();

		// Drives the pod at the given power (-1 to 1 like in FRC)
		void drive(double power);
	private:
		int sPin;
		Servo servo;
		Encoder encoder;
};

#endif