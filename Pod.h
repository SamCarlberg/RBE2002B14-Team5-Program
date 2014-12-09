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
		
		void drive(double power);

		// Drives the pod at the given speed.
		void driveRPM(double rpm);

		SingleInterruptEncoder encoder;

	private:
		Servo servo;
		int sPin;
		double calcPID(double input, double setpoint);
		double curSpeed;
		double p, i, d;
		double lastError;
};

#endif