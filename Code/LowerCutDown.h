#pragma once

#include "BatterySense.h"
#include "GPOinterface.h"

#define S_MONITORING 		0
#define S_ARMED 				1
#define S_PREPPING 			2
#define S_EXECUTING 		3
#define S_COMPLETE 			4

class LowerCutDown {
private:
	GPOinterface *wire;
	BatterySense *battery;

public:
	unsigned int state;
	bool aboveArmAlt;
	bool aboveCutAlt;

	elapsedMillis timer;

	void init(GPOinterface *wire, BatterySense *battery);
	void update();
	void executeNow();
};