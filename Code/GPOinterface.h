#pragma once

class GPOinterface {
private:
	unsigned int pin;
	float maxVoltage;

public:
	void init(unsigned int pin, float maxVoltage);
	void on(float power, float batVolt);
	void off();
};