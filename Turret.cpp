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
double Kp = 8, Ki = 0, Kd = 0;
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
	// motor.attach(TURRET_MOTOR_PIN, 1000, 2000);
	motor.attach(TURRET_MOTOR_PIN);
	IRServo.attach(IR_SERVO_PIN);
}

double Turret::getAngle() {
	// return pot.getAngle();
	return mapDouble(analogRead(TURRET_POT_PIN), 902, 396, 0, 360);
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
	if(abs(error) <= 1) {
		accError  = 0; // reset PID
		lastError = 0; 
		motor.write(90);
		delay(100);
		return true;
	}
	accError += error;
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

boolean Turret::scan() {
	int tempValue = -1;

	double sum = 0;
	double xSum = 0;
	double ySum = 0;
	double xBar;
	double yBar;

	switch(scan_State){
		//Initialization of scan
		//
		case 0:
			scan_PosX = 0;
			scan_PosY = 0;
			scan_TurretAngle = 0;
			scan_ServoAngle = 0;
			Serial.println("Case 0: Initialization Complete");
			scan_State++;
			break;

		//here we set the turret angle to a negative number to compensate for slack
		case 1:
			if(setTurretAngle(-45)){
				scan_State++;
				// Serial.println("Case 1: Turret in -45 position");
			}
			break;

		//here we set the turret angle, and wait for it to return true
		case 2:
			if(setTurretAngle(scan_TurretAngle)){
				scan_State++;
				// Serial.println("Case 2: Turret in correct reading position");
			}
			break;
		//Then we set the servo angle
		case 3:
			setServoAngle(scan_ServoAngle);
			scan_State++;
			break;
		//here we have a delay to allow the servo to reach it's angle
		case 4:
			delay(SERVO_DELAY);
			scan_State++;
			// Serial.println("Case 3-4: Servo in correct reading position");
			break;
		//Then, after the servo and turret are both in place, we measure the flame sensor value
		//and add it to the 2d array
		case 5:
			tempValue = (1024 - 18) - analogRead(IR_SENSOR_PIN);
			scan_Value[scan_PosY][scan_PosX] = tempValue;
			scan_ServoAngle += SERVO_ANGLE_INCREMENT;
			scan_State++;
			// Serial.print("Case 5: Value Read: ");
			// Serial.println(tempValue);
			break;
		//here, we decide whether to:
			// increment the servo angle, 
			// reset the servo angle and increment the turret angle
			// reset both the servo angle and the turret angle, and continue with the calculation
		case 6:
			if(scan_ServoAngle > SERVO_MAX_ANGLE){
				scan_ServoAngle = 0;
				scan_PosY = 0;
				setServoAngle(0);
				scan_TurretAngle += TURRET_ANGLE_INCREMENT;
				if(scan_TurretAngle >= TURRET_MAX_ANGLE){
					scan_PosX = 0;
					scan_ServoAngle = 0;
					scan_TurretAngle = 0;
					scan_State++;
					// Serial.println("Case 6: Scan finished, values to be displayed");
				} else {
					scan_PosX++;
					scan_State = 2;
					// Serial.println("Case 6: Turret Repositioning");
				}
			} else {
				scan_PosY++;
				scan_State = 3;
			}
			break;
		//Now that we have completer the scan, we reset the turret
		case 7:
			if(setTurretAngle(-45)){
				scan_State++;
			}
			break;
		case 8:
			if(setTurretAngle(0)){
				scan_State++;
			}
			break;
		//Then we perform the calculation, and store the heading and pitch of the candle (somewhere)
		case 9:
			int i, j;

			for(i = 0; i < IR_DATA_ROWS; i++){
				for(j = 0; j < IR_DATA_COLS; j++){
					Serial.print(scan_Value[i][j]);
					Serial.print("  ");
				}
				Serial.println(";");
			}
			Serial.println(" ");
			Serial.println("Finished Scan");
			Serial.println();


			for(i = 0; i < IR_DATA_ROWS; i++){
				for(j = 0; j < IR_DATA_COLS; j++){
					sum += scan_Value[j][i];
					xSum += scan_Value[j][i] * i;
					ySum += scan_Value[j][i] * j;
				}
			}

			// xBar = (xSum / (double)sum) * (TURRET_MAX_ANGLE / (double)TURRET_ANGLE_INCREMENT);
			// yBar = (ySum / (double)sum) * (SERVO_MAX_ANGLE / (double)SERVO_ANGLE_INCREMENT);

			xBar = (xSum / sum);
			yBar = (ySum / sum);

			Serial.print("test: ");
			Serial.print(xBar);
			Serial.print("  ");
			Serial.println(yBar);

			xBar = (xSum / sum) * TURRET_ANGLE_INCREMENT;
			yBar = (ySum / sum) * SERVO_ANGLE_INCREMENT;

			Serial.print("test 2: ");
			Serial.print(xBar);
			Serial.print("  ");
			Serial.println(yBar);

			xBar = (xSum / sum) * (TURRET_MAX_ANGLE / (double)TURRET_ANGLE_INCREMENT);
			yBar = (ySum / sum) * (SERVO_MAX_ANGLE / (double)SERVO_ANGLE_INCREMENT);

			Serial.print("Candle located at: ");
			Serial.print(xBar);
			Serial.print("  ");
			Serial.println(yBar);

			Serial.print("X increments: ");
			Serial.print(IR_DATA_COLS);
			Serial.print("  Y increments: ");
			Serial.println(IR_DATA_ROWS);


			scan_State++;
			break;
		case 10:
			//Chillax
			break;

		//default case to covered in the event of an emergency
		default:
			scan_State = 0;
			break;
		
	}
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