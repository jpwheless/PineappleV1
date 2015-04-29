#pragma once

#include <Arduino.h>

#define V_REF			2.500
#define ADC_MAX		4095.0
#define BAT_VSNS_MULT	2.005

class BatterySense {
private:
	static const float voltLookup[];
	static const float capLookup[];
	static const int tblSize;

	unsigned int snsPin;
	unsigned int adcReading;
	float avgTConst;
	float voltMult;
	float capStart;

	float interpolate(float value, const float tableOne[], const float tableTwo[], const int tableSize);

public:
	float voltage;
	bool empty;

	void init(unsigned int batSensePin, float voltageMultiplier, float avgTConst);

	void read();

	float getCap();
};

