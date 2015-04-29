#pragma once

#include "BatterySense.h"
#include "GPOinterface.h"

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