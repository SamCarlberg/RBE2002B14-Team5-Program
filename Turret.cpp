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
double Kp = 2, Ki = 0.1, Kd = 0.000;
double accError = 0;
double lastError = 0;

Turret::Turret():
	pot(A0, 487, 110),
	rangeSensor(ULTRASONIC_TRIGGER_PIN, ULTRASONIC_ECHO_PIN){
}

void Turret::init() {
	motor.attach(TURRET_MOTOR_PIN, 1000, 2000);
	Serial.println(IR_DATA_ROWS);
	Serial.println(IR_DATA_COLS);
}

double Turret::getAngle() {
	// return pot.getAngle();
	return mapDouble(analogRead(TURRET_POT_PIN), 105, 861, 0, 180);
}

double Turret::getDistance() {
	return rangeSensor.getRangeInches();
}

Point Turret::getObstacleLocation() {
	double dist = getDistance();
	double angle = getAngle();
	double x = dist * cos(toRad(angle));
	double y = dist * sin(toRad(angle));
	Serial.print("("); Serial.print(angle); Serial.print(", "); Serial.print(dist); Serial.print(")\t->\t");
	Serial.print("("); Serial.print(x); Serial.print(", "); Serial.print(y); Serial.print(")\n");
	Point p(x, y);
	obstacles[curAngle * ((MAX_ANGLE - MIN_ANGLE) / ANGLE_INCREMENT)] = &p;
	return p;
}
boolean Turret::setAngle(double angle) {
	double error = angle - getAngle();
	if(abs(error) <= 0.5) {
		accError  = 0; // reset PID
		lastError = 0; 
		motor.write(90);
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


boolean Turret::scan() {
	if(resetting) {
		curAngle = MIN_ANGLE;
		resetting = !setAngle(curAngle); // if we're not there, then we're still resettting
		return resetting;
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