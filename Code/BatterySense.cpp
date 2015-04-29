#include "BatterySense.h"

// Search for value in tableOne, then interpolate between tableTwo
// Assumes tableOne is sorted in ascending order
float BatterySense::interpolate(float value, const float tableOne[], const float tableTwo[], const int tableSize) {
	if (value < tableOne[0]) return tableTwo[0];
	if (value > tableOne[tableSize-1]) return tableTwo[tableSize-1];

	int first = 0;
	int last = tableSize - 1;
	int middle;

	// Binary search to find the table values bracketing the lookup value
	while (last - first > 1) {
		middle = (last + first)/2;
		if (value > tableOne[middle])
			first = middle;
		else if (value < tableOne[middle]) last = middle;
		else return capLookup[middle];
	}

	// Linear interpolation
	return tableTwo[first] + (tableTwo[last] - tableTwo[first])*(value - tableOne[first])/(tableOne[last] - tableOne[first]);
}

void BatterySense::init(unsigned int batSensePin, float voltageMultiplier, float avgTConst) {
	this->snsPin = batSensePin;
	this->voltMult = voltageMultiplier;
	if (avgTConst > 0.0 && avgTConst <= 100.0) this->avgTConst = 0.63212*(1.0/avgTConst);
	else this->avgTConst = 0.063212;

	float thisVoltage = 0.0;
	for (int i = 0; i <5; i++) {
		thisVoltage += (V_REF/ADC_MAX)*voltMult*float(analogRead(snsPin));
	}
	voltage = (thisVoltage/5.0);

	// Assume that this was taken when millis() was roughly equal to zero
	capStart = getCap();
}

void BatterySense::read() {
	static elapsedMicros timeSinceLastCall = 1000; // Guess at average loop time

	float thisVoltage = (V_REF/ADC_MAX)*voltMult*float(analogRead(snsPin));
	float tc = avgTConst*1.0e-6*float(timeSinceLastCall);

	if (tc < 1.0) // Needs to be less than time constant for calculation to make sense
		voltage = (1.0-tc)*voltage + tc*thisVoltage;
	else voltage = thisVoltage;

	if (empty && voltage > 3.0) empty = false;
	else if (voltage < 2.8) empty = true;
}

float BatterySense::getCap() { // Interpolate lookup tables for accurate capacity
	return interpolate(voltage, voltLookup, capLookup, tblSize);
}


// Specifically for Panasonic NCR18650B
const float BatterySense::voltLookup[] = { // Assumes < 0.2A current draw
	2.805,2.900,2.984,3.075,3.150,3.192,3.241,3.274,3.301,3.327,3.367,3.406,3.430,3.453,
	3.478,3.495,3.511,3.530,3.541,3.558,3.571,3.583,3.599,3.616,3.630,3.653,3.674,3.697,
	3.725,3.753,3.774,3.799,3.823,3.860,3.888,3.909,3.939,3.967,4.016,4.053,4.081,4.107,4.156};
const float BatterySense::capLookup[] = { // Assumes < 0.2A current draw
	0.000,0.097,0.644,1.396,2.285,2.969,4.336,6.593,9.738,12.06,14.66,17.33,19.38,21.84,
	24.78,26.90,29.16,32.24,34.29,37.09,39.83,41.88,44.34,47.28,49.60,52.34,54.87,57.19,
	60.00,62.32,64.72,67.25,69.98,74.22,77.37,79.69,82.49,85.02,88.79,92.48,95.28,97.67,100.0};
const int BatterySense::tblSize = 43;
