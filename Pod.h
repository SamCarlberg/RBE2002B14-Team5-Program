/*

	Class representing a swerve pod, containing a motor and encoder.

 */
#ifndef Pod_h
#define Pod_h

#include <Servo.h>
#include <SingleInterruptEncoder.h>

class Pod {
	public:
		// Constructor.
		Pod(int motorPin);

		// Attaches the servo.
		void init();

		// Gets the distance travelled (in inches) by the pod
		double getDistance();

		// Drives the pod at the given power (-1 to 1 like in FRC)
		void drive(double power);

		SingleInterruptEncoder encoder;
	private:
		int sPin;
		Servo servo;
};

#endif