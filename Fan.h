/*

	Class representing the brushless motor used to drive the fan

*/
#ifndef Fan_h
#define Fan_h

#include <Servo.h>
#include <Constants.h>

class Fan {
	public:
		//constructor
		Fan();

		//attaches and arms the brushless motor
		void init();

		//ramps up the speed of the fan
		void speedUp();

		//ramps down the speed of the fan
		void slowDown();

		//gets the current speed of the motor
		int getSpeed();

		//returns whether the motor is stopped
		boolean isStopped();

		//returns whether the motor is at its max speed
		boolean isAtMaxSpeed();
	private:
		Servo fanMotor;
		int fPin;
		int curSpeed;
		long prevMillis;
		void arm();
		void setSpeed(int speed);
};

#endif