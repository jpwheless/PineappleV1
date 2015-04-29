#pragma once

#include "GPOinterface.h"
#include "BatterySense.h"
#include "LowerCutDown.h"

class UpperCutDown {
private:
	GPOinterface *wire;
	BatterySense *battery;
	LowerCutDown *lowerCut;

public:
	unsigned int state;
	bool maxAltIncreased;
	bool outsideBoundaries;
	bool aboveArmAlt;
	bool falling;

	elapsedMillis cutTimer;
	elapsedMillis timerA;
	elapsedMillis timerB;

	void init(GPOinterface *wire, BatterySense *battery, LowerCutDown *lowerCut);
	void update();
	void executeNow();
	void prepNow();
};