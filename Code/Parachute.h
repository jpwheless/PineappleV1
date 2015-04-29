#pragma once

#include "GPOinterface.h"
#include "BatterySense.h"
#include "UpperCutDown.h"

class Parachute {
private:
	GPOinterface *wire;
	BatterySense *battery;
	UpperCutDown *upperCut;

public:
	unsigned int state;
	bool belowMaxAlt;			// Arming
	bool falling;					// Arming and Exec
	bool belowDeployAlt;	// Executing

	elapsedMillis timer;

	void init(GPOinterface *wire, BatterySense *battery, UpperCutDown *upperCut);
	void update();
	void executeNow();
};