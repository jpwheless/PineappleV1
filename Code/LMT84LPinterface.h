#pragma once

// LMT84LP analog thermometer interface
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