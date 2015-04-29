#include "Heater.h"

void Heater::init(unsigned int heaterPin, LMT84LPinterface *thermometer, float target, float cP, float cI) {
	this->heaterPin = heaterPin;
	this->thermometer = thermometer;
	this->target = target;
	this->cP = cP;
	this->cI = cI;
}

void Heater::update() {
	static elapsedMicros timeSinceLastCall = 1000; // Guess at average loop time

	thermometer->read();
	error = target - thermometer->celsius;
	integral = constrain(integral + error*cI*timeSinceLastCall*1.0e-6, -1.0, 1.0);
	dutyCycle = constrain(error*cP + integral, 0.0, 1.0);

	analogWrite(heaterPin, int(PWM_MAX*dutyCycle + 0.5));
}