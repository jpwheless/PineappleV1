#pragma once

#include <Arduino.h>

#define PWM_MAX		2047.0

class LEDinterface {
private:
	unsigned int pin;

public:
	void init(unsigned int pin);
	void write(unsigned int value);
};