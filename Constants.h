/*
 *  This file contains all the constants used in the program.
 */

#ifndef Constants_h
#define Constants_h

/*
 *  Helper functions
 */
// Helper function to get the sign of a number
#define sign(x) ((x) < 0 ? -1 : (x) > 0 ? 1 : 0)

// Squares a number and keeps its sign
#define sq(x) ((x) * (x) * sign(x))

#define isNullPoint(p) (p.x == 0 && p.y == 0)

struct Point {
	Point(double a, double b): x(a), y(b){}
	double x, y;
};

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
	FR_ENC_PIN	= 1,
	FL_ENC_PIN	= 2,
	RR_ENC_PIN	= 3,
	RL_ENC_PIN	= 4
};

const int SWERVE_POT_PIN = A11;

/*
 *  Encoder constants
 */
const double DEGREES_PER_ENC_TICK = 2; // 180 ticks per rev (counting rising and falling edge)

/*
 *  Turret constants.
 */
const int TURRET_MOTOR_PIN = 9; // Change this to match the physical robot

/*
 *  Map constants. Tune these!
 */
const static byte FIELD_WIDTH  = 72; // inches
const static byte FIELD_HEIGHT = 72;


/*
 *  State machine constants.
 */
enum STATE {
	START,
	MOVING,
	SCANNING,
	CALCULATING,
	EXTINGUISHING,
	RETURNING,
	COMPLETE
};

#endif