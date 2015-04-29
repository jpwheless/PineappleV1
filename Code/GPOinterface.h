#pragma once

#include <Heater.h>

#define PWM_MAX		2047.0

class GPOinterface {
private:
	unsigned int pin;
	float maxVoltage;

public:
	void init(unsigned int pin, float maxVoltage);
	void on(float power, float batVolt);
	void off();
};