#include "LMT84LPinterface.h"

// Assign signal pin and the time constant in seconds for readAverage()
// Set time constant to logging interval
void LMT84LPinterface::init(unsigned int sigPin, float tConst, float cor1, float cor2) {
	this->sigPin = sigPin;
	this->cor1 = cor1;
	this->cor2 = cor2;
	pinMode(sigPin, INPUT);

	if (avgTConst > 0.0 && avgTConst <= 10.0) this->avgTConst = 0.63212*(1.0/tConst);
	else this->avgTConst = 0.63212;

	// Initialize average
	float thisVoltage = 0.0;
	for (int i = 0; i <5; i++) {
		thisVoltage += cor1*(1000.0*V_REF/ADC_MAX)*float(analogRead(sigPin)) + cor2;
	}
	millivolts = (thisVoltage/5.0);
	celsius = (5.506 - sqrt(30.316 + .00704*(870.6 - millivolts)))/(-.00352) + 30.0;
}

// Updates class temperature variable
// Call many times between logging to SD card for best sampling
void LMT84LPinterface::read() {
	static elapsedMicros timeSinceLastCall = 1000; // Guess at average loop time

	float thisVolt = cor1*(1000.0*V_REF/ADC_MAX)*float(analogRead(sigPin)) + cor2;
	float tc = avgTConst*1.0e-6*float(timeSinceLastCall);

	if (tc < 1.0) // Needs to be less than time constant for calculation to make sense
		millivolts = (1.0-tc)*millivolts + tc*thisVolt;
	else millivolts = thisVolt;

	celsius = (5.506 - sqrt(30.316 + .00704*(870.6 - millivolts)))/(-.00352) + 30.0;
}