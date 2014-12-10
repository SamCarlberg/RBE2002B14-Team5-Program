/*
 *  This file contains all the constants used in the program.
 */

#ifndef Constants_h
#define Constants_h

/*
 *  Helper macro function(s)
 */
#define isNullPoint(p) (p.x == 0 && p.y == 0)
#define sign(x) ((x) < 0 ? -1 : (x) > 0 ? 1 : 0)
#define toRad(deg) ((deg) * PI / 180.0)

struct Point {
	Point(double a, double b): x(a), y(b){}
	double x, y;
};

/*
 *  Swerve drive constants. Change these!
 */
enum SwerveServoPins {
	CENTER 	= 4,	// motor that turns the pods
	FR_MOTOR_PIN 		= 5,
	FL_MOTOR_PIN 		= 8,
	RL_MOTOR_PIN 		= 7,
	RR_MOTOR_PIN 		= 6
};

enum SwerveEncoderPins {
	FR_ENC_PIN	= 1, // pin 3
	FL_ENC_PIN	= 0, // pin 2
	RR_ENC_PIN	= 4, // pin 19
	RL_ENC_PIN	= 5  // pin 18
};

const int SWERVE_POT_PIN = A11;

/*
 *  Encoder constants
 */
const double DEGREES_PER_ENC_TICK = 2.0; // 180 ticks per rev (counting rising and falling edge)

/*
 *  Turret constants.
 */
const int TURRET_MOTOR_PIN = 4; // Change this to match the physical robot

/*
 * Fan constant(s)
 */
const int FAN_MOTOR_PIN = 8;//change this to match the physical robot

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