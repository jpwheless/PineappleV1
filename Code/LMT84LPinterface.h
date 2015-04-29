#pragma once

#include <Arduino.h>

#define V_REF			2.500
#define ADC_MAX		4095.0
#define BAT_VSNS_MULT	2.005

class LMT84LPinterface {
private:
	unsigned int sigPin;
	float avgTConst;
	float averageTemp;
	float cor1;
	float cor2;

public:
	float millivolts;
	float celsius;

	void init(unsigned int sigPin, float tConst, float cor1, float cor2);
	void read();
};