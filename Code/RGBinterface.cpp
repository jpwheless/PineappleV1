#include "RGBinterface.h"

void RGBinterface::init(unsigned int pinRed, unsigned int pinGreen, unsigned int pinBlue, bool sinkDrive) {
	this->pinRed = pinRed;
	this->pinGreen = pinGreen;
	this->pinBlue = pinBlue;
	this->sinkDrive = sinkDrive;

	pinMode(pinRed, OUTPUT);
	pinMode(pinGreen, OUTPUT);
	pinMode(pinBlue, OUTPUT);

	color(0,0,0);
}

// Maps 0-255 inputs to exponential pwm outputs
void RGBinterface::color(int r, int g, int b) {
	if (sinkDrive) {
		if (r >= 255) analogWrite(pinRed, 0);
		else if (r <= 0) analogWrite(pinRed, PWM_MAX);
		else if (r <= 4) analogWrite(pinRed, PWM_MAX-1);
		else {
			analogWrite(pinRed, PWM_MAX - int((PWM_MAX/65025.0)*pow(r, 2.0) + 0.5));
		}

		if (g >= 255) analogWrite(pinGreen, 0);
		else if (g <= 0) analogWrite(pinGreen, PWM_MAX);
		else if (g <= 4) analogWrite(pinGreen, PWM_MAX-1);
		else {
			analogWrite(pinGreen, PWM_MAX - int((PWM_MAX/65025.0)*pow(g, 2.0) + 0.5));
		}

		if (b >= 255) analogWrite(pinBlue, 0);
		else if (b <= 0) analogWrite(pinBlue, PWM_MAX);
		else if (b <= 4) analogWrite(pinBlue, PWM_MAX-1);
		else {
			analogWrite(pinBlue, PWM_MAX - int((PWM_MAX/65025.0)*pow(b, 2.0) + 0.5));
		}
	}
	else {
		if (r >= 255) analogWrite(pinRed, PWM_MAX);
		else if (r <= 0) analogWrite(pinRed, 0);
		else if (r <= 4) analogWrite(pinRed, 1);
		else {
			analogWrite(pinRed, int((PWM_MAX/65025.0)*pow(r, 2.0) + 0.5));
		}

		if (g >= 255) analogWrite(pinGreen, PWM_MAX);
		else if (g <= 0) analogWrite(pinGreen, 0);
		else if (g <= 4) analogWrite(pinGreen, 1);
		else {
			analogWrite(pinGreen, int((PWM_MAX/65025.0)*pow(g, 2.0) + 0.5));
		}

		if (b >= 255) analogWrite(pinBlue, PWM_MAX);
		else if (b <= 0) analogWrite(pinBlue, 0);
		else if (b <= 4) analogWrite(pinBlue, 1);
		else {
			analogWrite(pinBlue, int((PWM_MAX/65025.0)*pow(b, 2.0) + 0.5));
		}
	}
}