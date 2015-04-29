#include "GPOinterface.h"

void GPOinterface::init(unsigned int pin, float maxVoltage) {
	this->pin = pin;
	this->maxVoltage = constrain(maxVoltage, 0.0, 5.0);

	pinMode(pin, OUTPUT);
	off();
}

void GPOinterface::on(float power, float batVolt) {
	unsigned int maxPWM = PWM_MAX*(maxVoltage/batVolt) + 0.5;

	if (power >= 1.00)
		analogWrite(pin, maxPWM);
	else if (power <= 0.00)
		analogWrite(pin, 0);
	else
		analogWrite(pin, int(maxPWM*power + 0.5));
}

void GPOinterface::off() {
	analogWrite(pin, 0);
}