#include <Arduino.h>
#include <Constants.h>
#include <Turret.h>

#define IR_READING_FLAME_THRESHOLD 450

// helper function
double mapDouble(double in, double minIn, double maxIn, double minOut, double maxOut){
	return ((in - minIn) * (maxOut - minOut) / (maxIn - minIn) + minOut);
} 

// variables for setAngle()
int curAngle = MIN_ANGLE;
boolean resetting = false;

// PID variables
double Kp = 4.6, Ki = 0.022, Kd = 0.008;
double accError = 0;
double lastError = 0;

Turret::Turret():
	pot(A0, 487, 110),
	rangeSensor(ULTRASONIC_TRIGGER_PIN, ULTRASONIC_ECHO_PIN){
}

void Turret::init() {
	// motor.attach(TURRET_MOTOR_PIN, 1000, 2000);
	motor.attach(TURRET_MOTOR_PIN);
}

double Turret::getAngle() {
	// return pot.getAngle();
	return mapDouble(analogRead(TURRET_POT_PIN), 116, 864, 0, 180);
}

double Turret::getDistance() {
	return rangeSensor.getRangeInches();
}

void Turret::getObstacleLocation() {
	double dist = getDistance() + BOOM_LENGTH;
	double angle = getAngle();
	double x = dist * cos(toRad(angle));
	double y = dist * sin(toRad(angle));
	obstacleXVals[curAngle / ANGLE_INCREMENT - 1] = x;
	obstacleYVals[curAngle / ANGLE_INCREMENT - 1] = y;
}

boolean Turret::setAngle(double angle) {
	double error = angle - getAngle();
	if(abs(error) <= 1 || angle <= MIN_ANGLE || angle >= MAX_ANGLE) {
		accError  = 0; // reset PID
		lastError = 0; 
		motor.write(90);
		delay(100);
		return true;
	}
	accError += error;
	double power = Kp * error + Ki * accError + Kd * (error - lastError);
	lastError = error;
	power = constrain(power + 90, 0, 180);
	motor.write(power);
	delay(10);
	return false;
}


boolean hitMax = false;
boolean Turret::scan() {
	if(resetting) {
		curAngle = MIN_ANGLE;
		resetting = !setAngle(curAngle); // if we're not there, then we're still resettting
		return !resetting;
	}
	if(setAngle(curAngle)) {
		if(abs(curAngle - MAX_ANGLE) <= 0.5) {
			getObstacleLocation();
			Serial.println("Finished scan!");
			resetting = true;
		} else {
			curAngle += ANGLE_INCREMENT;
			getObstacleLocation();
			setAngle(curAngle);
		}
	}
	return false;
}

boolean Turret::processIRData() {
	boolean foundFlame = false;
	Point highest(0, 0);
	foundFlame = false;
	for(int x = 0; x < IR_DATA_ROWS; x++) {
		for(int y = 0; y < IR_DATA_COLS; y++) {
			uint16_t val = irData[x][y];
			if(val < IR_READING_FLAME_THRESHOLD) { // found a potential flame
				foundFlame = true;
				if(val < irData[(int) highest.x][(int) highest.y]) {
					highest.x = x;
					highest.y = y;
				}
			}			
		}
	}
	return foundFlame;
}

void Turret::printObstacles() {
	for (int i = 0; i < ((MAX_ANGLE - MIN_ANGLE) / ANGLE_INCREMENT); ++i) {
		Serial.print("(");
		Serial.print(obstacleXVals[i]);
		Serial.print(", ");
		Serial.print(obstacleYVals[i]);
		Serial.print(")  ");
	}
	Serial.print('\n');
}