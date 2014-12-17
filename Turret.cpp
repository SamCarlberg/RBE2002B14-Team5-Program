#include <Arduino.h>
#include <Constants.h>
#include <Turret.h>

#define IR_READING_FLAME_THRESHOLD 450

// helper function
double mapDouble(double in, double minIn, double maxIn, double minOut, double maxOut){
	return ((in - minIn) * (maxOut - minOut) / (maxIn - minIn) + minOut);
} 

// variables for setTurretAngle()
int curAngle = TURRET_MIN_ANGLE;
boolean resetting = false;

// PID variables
// double Kp = 4.6, Ki = 0.022, Kd = 0.008;
double Kp = -2, Ki = -0.1, Kd = -1.0;
double accError = 0;
double lastError = 0;

Turret::Turret():
	pot(A0, 487, 110),
	rangeSensor(ULTRASONIC_TRIGGER_PIN, ULTRASONIC_ECHO_PIN){
		scan_State = 0;
		scan_PosX = 0;
		scan_PosY = 0;
		scan_TurretAngle = 0;
		scan_ServoAngle = 0;
}

void Turret::init() {
	motor.attach(TURRET_MOTOR_PIN, 1000, 2000);
	// motor.attach(TURRET_MOTOR_PIN);
	IRServo.attach(IR_SERVO_PIN);
}

double Turret::getAngle() {
	// return pot.getAngle();
	return mapDouble(analogRead(TURRET_POT_PIN), TURRET_POT_0ANGLE, TURRET_POT_360ANGLE, 0, 360);
}

double Turret::getDistance() {
	return rangeSensor.getRangeInches();
}

void Turret::getObstacleLocation() {
	double dist = getDistance() + BOOM_LENGTH;
	double angle = getAngle();
	double x = dist * cos(toRad(angle));
	double y = dist * sin(toRad(angle));
	obstacleXVals[curAngle / TURRET_ANGLE_INCREMENT - 1] = x;
	obstacleYVals[curAngle / TURRET_ANGLE_INCREMENT - 1] = y;
}

boolean Turret::setTurretAngle(double angle) {
	double error = constrain(angle, TURRET_MIN_LIMIT, TURRET_MAX_LIMIT) - getAngle();
	// Serial.println(error);
	if(abs(error) < TURRET_ERROR_THRESHOLD) {
		delay(100);
		error = constrain(angle, TURRET_MIN_LIMIT, TURRET_MAX_LIMIT) - getAngle();
		if(abs(error) < TURRET_ERROR_THRESHOLD) {
			accError  = 0; // reset PID
			lastError = 0; 
			motor.write(90);
			delay(100);
			return true;
		}
	}
	if(abs(error) < TURRET_ANGLE_INCREMENT) {
		accError += error;
	}
	double power = Kp * error + Ki * accError + Kd * (error - lastError);
	lastError = error;
	power = constrain(power + 90, 0 + TURRET_POWERLIMIT, 180 - TURRET_POWERLIMIT);
	motor.write(power);
	delay(10);
	return false;
}

void Turret::setServoAngle(double angle){
	double outAngle = constrain(map(angle, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE, SERVO_MIN_ANGLE_OFFSET, SERVO_MAX_ANGLE_OFFSET), 0, 180);
	IRServo.write(outAngle);
	// IRServo.write(angle);
}


// boolean hitMax = false;
// boolean Turret::scan() {
// 	if(resetting) {
// 		curAngle = MIN_ANGLE;
// 		resetting = !setTurretAngle(curAngle); // if we're not there, then we're still resettting
// 		return !resetting;
// 	}
// 	if(setTurretAngle(curAngle)) {
// 		if(abs(curAngle - MAX_ANGLE) <= 0.5) {
// 			getObstacleLocation();
// 			Serial.println("Finished scan!");
// 			resetting = true;
// 		} else {
// 			curAngle += ANGLE_INCREMENT;
// 			getObstacleLocation();
// 			setTurretAngle(curAngle);
// 		}
// 	}
// 	return false;
// }

boolean Turret::scan(){
	scan(0, 360);
}

boolean Turret::scan(double inputMinTurretAngle, double inputMaxTurretAngle) {
	int tempValue = -1;
	boolean isFinished = false;
	
	switch(scan_State){

		//Initializing scan variables
		case 0:
			scan_PosX = 0;
			scan_PosY = 0;

			scan_TurretAngle = inputMinTurretAngle;
			scan_ServoAngle = SERVO_MIN_ANGLE;

			scan_Sum = 0;
			scan_XSum = 0;
			scan_YSum = 0;

			scan_State++;

			// Serial.println("Case 0");
			break;

		//here we set the turret angle to a negative number to compensate for slack
		case 1:
			if(setTurretAngle(inputMinTurretAngle - 15)){
				scan_State++;

				// Serial.println("Case 1");
			}
			break;

		//Here we set the turret angle, and wait for it to return true
		case 2:
			if(setTurretAngle(scan_TurretAngle)){
				scan_State++;

				// Serial.println("Case 2");
			}
			break;

		//Then we set the servo position and wait for it to reach its position
		case 3:
			setServoAngle(scan_ServoAngle);
			delay(SERVO_DELAY);

			scan_State++;

			// Serial.println("Case 3");
			break;

		//Then, after the servo and turret are both in place, we measure the flame sensor value
		//and add it to the calculus variables
		case 4:
			tempValue = pow(((1024) - analogRead(IR_SENSOR_PIN)), 2) / 10000;
			Serial.print(tempValue);
			Serial.print(" ");
			// scan_Value[scan_PosX][scan_PosY] = tempValue;
			scan_Sum += tempValue;
			scan_XSum += tempValue * (scan_PosX);
			scan_YSum += tempValue * (scan_PosY);

			scan_ServoAngle += SERVO_ANGLE_INCREMENT;
			scan_PosY++;

			scan_State++;

			// Serial.println("Case 4");
			break;

		//Here, we decide whether to:
			// Increment the servo angle, 
			// Reset the servo angle and increment the turret angle
			// Reset both the servo angle and the turret angle, and continue with the calculation
		case 5:
			if(scan_ServoAngle > SERVO_MAX_ANGLE){
				Serial.println();

				scan_ServoAngle = 0;
				scan_PosY = 0;

				setServoAngle(SERVO_MIN_ANGLE);

				scan_TurretAngle += TURRET_ANGLE_INCREMENT;
				scan_PosX++;

				if(scan_TurretAngle >= inputMaxTurretAngle){
					scan_PosX = 0;
					scan_ServoAngle = 0;
					scan_TurretAngle = 0;

					scan_State++;

				} else {

					//after incrementing the turret angle, we reset it and continue reading values
					scan_State = 2;

				}
			} else {

				//after incrementing the servo angle, we take more values
				scan_State = 3;
			}
			break;

		//Here we perform the calculation, and store the heading and pitch of the candle (somewhere)
		case 6:

			scan_XBar = (scan_XSum / scan_Sum) * TURRET_ANGLE_INCREMENT + inputMinTurretAngle + TURRET_ANGLE_OFFSET;
			scan_YBar = (scan_YSum / scan_Sum) * SERVO_ANGLE_INCREMENT + SERVO_MIN_ANGLE + SERVO_ANGLE_OFFEST;

			scan_State++;

			break;

		//Here we point the turret in the direction of the flame
		//First we move it back to get ride of slack
		case 7:
			if(setTurretAngle(scan_XBar - 15)){
				scan_State++;
			}
			break;

		//Then we move it to the position calculated in the flame
		case 8:
			if(setTurretAngle(scan_XBar)){
				setServoAngle(scan_YBar);
				scan_State++;
			}
			break;

		//When the turret is pointed in the right direction,
		//The robot moves on to the next task
		case 9:
			//Chillax
			isFinished = true;
			scan_State = 0;
			break;

		//default case to covered in the event of an emergency
		default:
			scan_State = 0;
			break;
	
	}
	return isFinished;
}

//This is the quickScan function, it does a ver fast scan
//and returns either the angle where the candle may be
//or -1 if no candle was found
double Turret::quickScan(){
	double tempAngle = 0;
	int tempValue = 0;

	Serial.println(scan_State);

	switch(scan_State){

		//Initializing scan variables
		case 0:
			scan_TurretAngle = TURRET_MIN_ANGLE;
			setServoAngle(SERVO_QUICK_ANGLE);

			current_Highest = 0;
			current_HighestAngle = -1;

			scan_State++;
			break;

		//here we set the turret angle to a negative number to compensate for slack
		case 1:
			if(setTurretAngle(TURRET_MIN_ANGLE - 15)){
				scan_State++;
			}
			break;

		//Here we set the turret angle, and wait for it to return true
		case 2:
			if(setTurretAngle(scan_TurretAngle)){
				scan_State++;
			}
			break;

		//Then, after the servo and turret are both in place, we measure the flame sensor value
		//and add it to the calculus variables
		case 3:
			tempValue = (1024 - 18) - analogRead(IR_SENSOR_PIN);

			if(tempValue > current_Highest && tempValue > IR_THRESHOLD){
				current_Highest = tempValue;
				current_HighestAngle = scan_TurretAngle;
			}

			if(scan_TurretAngle >= TURRET_MAX_ANGLE){
				scan_State++;
			} else {
				scan_TurretAngle += TURRET_ANGLE_INCREMENT;
				scan_State = 2;
			}
			break;

		case 4:
			tempAngle = current_HighestAngle;
			scan_State = 0;
			break;

		//default case to covered in the event of an emergency
		default:
			scan_State = 0;
			break;
	
	}

	return tempAngle;
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
	for (int i = 0; i < ((TURRET_MAX_ANGLE - TURRET_MIN_ANGLE) / TURRET_ANGLE_INCREMENT); ++i) {
		Serial.print("(");
		Serial.print(obstacleXVals[i]);
		Serial.print(", ");
		Serial.print(obstacleYVals[i]);
		Serial.print(")  ");
	}
	Serial.print('\n');
}