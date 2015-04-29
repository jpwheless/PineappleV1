#pragma once

class Heater {
private:
	LMT84LPinterface *thermometer;
	unsigned int heaterPin;
	float cP;
	float cI;

public:
	float target;
	float error;
	float dutyCycle;
	float integral;

	void init(unsigned int heaterPin, LMT84LPinterface *thermometer, float target, float cP, float cI);
	void update();
};