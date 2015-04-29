#include "LEDinterface.h"

void LEDinterface::init(unsigned int pin) {
		this->pin = pin;
		pinMode(pin, OUTPUT);

		write(0);
	}

	void LEDinterface::write(unsigned int value) {
		if (value >= 255) analogWrite(pin, PWM_MAX);
		else if (value <= 0) analogWrite(pin, 0);
		else if (value <= 4) analogWrite(pin, 1);
		else {
			analogWrite(pin, int((PWM_MAX/65025.0)*pow(value, 2.0) + 0.5));
		}
	}