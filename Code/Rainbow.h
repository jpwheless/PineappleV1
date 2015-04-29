#pragma once

#include "RGBinterface.h"

class Rainbow {
private:
	elapsedMicros timer;
	RGBinterface *rgb;
	int currentColor;
	int i;

public:
	bool active;

	void init(RGBinterface *rgb);
	void cycle();
	void on();
	void off();
};