#pragma once

#include "GPOinterface.h"
#include "BatterySense.h"
#include "UpperCutDown.h"

#define S_MONITORING 		0
#define S_ARMED 				1
#define S_PREPPING 			2
#define S_EXECUTING 		3
#define S_COMPLETE 			4

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