/*
 *  This file contains all the constants used in the program.
 */

#ifndef Constants_h
#define Constants_h

/*
 *  Swerve drive constants. Change these!
 */
enum SwerveServoPins {
	CENTER 	= 9,	// motor that turns the pods
	FR_MOTOR_PIN 		= 10,
	FL_MOTOR_PIN 		= 11,
	RL_MOTOR_PIN 		= 12,
	RR_MOTOR_PIN 		= 13
};

enum SwerveEncoderPins {
	FR_ENC_PIN	= 4,
	FL_ENC_PIN	= 5,
	RR_ENC_PIN	= 6,
	RL_ENC_PIN	= 7
};

const int SWERVE_POT_PIN = A11;

const double DEGREES_PER_ENC_TICK = 4;

#endif