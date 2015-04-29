#pragma once

#include <Arduino.h>
#include "GPOinterface.h"
#include "BatterySense.h"
#include "LowerCutDown.h"

#define S_MONITORING 		0
#define S_ARMED 				1
#define S_PREPPING 			2
#define S_EXECUTING 		3
#define S_COMPLETE 			4

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