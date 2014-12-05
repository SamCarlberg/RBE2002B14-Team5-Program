
#include <Servo.h>
#include <PPM.h>
#include <math.h>

PPM ppm(2);

int Controller_LeftJoystickVertical = 3;
int Controller_LeftJoystickHorizontal = 4;
int Controller_RightJoystickVertical = 2;
int Controller_RightJoystickHorizontal = 1;
int Controller_LeftButton = 5;
int Controller_RightButton = 6;

Servo NorthEast;
Servo NorthWest;
Servo SouthEast;
Servo SouthWest;
Servo PodMotor;

int NorthEastPin = 5;
int NorthWestPin = 8;
int SouthEastPin = 6;
int SouthWestPin = 7;
int PodMotorPin = 4;

String potPin = "A11";

double x = 90;
double y = 90;
double dist = 0;
double angle = 0;
double motorSpeed = 0;

double r1 = 20;
double r2 = 35;

double Kp = -5;

// void setup()
// {
// 	Serial.begin(9600);

// 	NorthEast.attach(NorthEastPin, 1000, 2000);
// 	NorthWest.attach(NorthWestPin, 1000, 2000);
// 	SouthEast.attach(SouthEastPin, 1000, 2000);
// 	SouthWest.attach(SouthWestPin, 1000, 2000);

// 	PodMotor.attach(PodMotorPin, 1000, 2000);
// }

void controllerDrive()
{
	x = ppm.getChannel(Controller_LeftJoystickHorizontal) - 90;
	y = -ppm.getChannel(Controller_LeftJoystickVertical) + 90;

	dist = pow((square(x) + square(y)), 0.5);

	if(dist > r1){
		angle = atan2(y, x)*180/3.14;
		//set angle of pods
	}

	if(dist > r2){
		motorSpeed = constrain(map(dist, r2, 75, 90, 0), 0, 90);
	} else {
		motorSpeed = 90;
	}

	double currentError = driveToAngle();
	if(abs(currentError) < 10){
		driveRobot();
	} else {
		correctRobot(currentError);
	}

	// PodMotor.write(ppm.getChannel(2));

	Serial.print("Angle: ");
	Serial.print(angle);
	Serial.print(", Error: ");
	Serial.println(driveToAngle());
	// Serial.print(", Dist: ");
	// Serial.print(dist);
	// Serial.print(", motorSpeed: ");
	// Serial.println(motorSpeed);
}

double driveToAngle(){
	double error = getError();

	double podSpeed = constrain(error*Kp + 90, 0, 180);
	PodMotor.write(podSpeed);

	double motorCorrection = (constrain(error*Kp + 90, 0, 180) - 90)/5.0 + 90;

	// NorthEast.write(motorCorrection);
	// NorthWest.write(motorCorrection);
	// SouthEast.write(motorCorrection);
	// SouthWest.write(motorCorrection);

	return error;
}

double getError(){
	double error = angle - potAngle();

	return error;
}

double potAngle(){
	int potReading = analogRead(A11);

	return map(potReading, 0, 1023, 360, -360);
}

double driveRobot(){
	NorthEast.write(motorSpeed);
	NorthWest.write(motorSpeed);
	SouthEast.write(motorSpeed);
	SouthWest.write(motorSpeed);
}

double correctRobot(double inError){
	double motorCorrection = (constrain(inError*Kp + 90, 0, 180) - 90)/5.0 + 90;

	NorthEast.write(motorCorrection);
	NorthWest.write(motorCorrection);
	SouthEast.write(motorCorrection);
	SouthWest.write(motorCorrection);
}











