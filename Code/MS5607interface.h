#pragma once

#include <Arduino.h>
#include "i2c_t3.h"

#define MS5607_RST		0x1E
#define MS5607_PCONV	0x48 // 4096 OSR (9.04ms)
#define MS5607_TCONV	0x58 // 4096 OSR (9.04ms)
#define MS5607_AREAD	0x00
#define MS5607_PREAD	0xA0 // Address of first PROM

class MS5607interface {
private:
	unsigned int readType;
	int64_t cal[6];

	unsigned int read();

public:
	float kPa;
	float celsius;
	float altitude;
	float rateOfClimb;
	float maxAlt;
	float maxAltIncreased;
	bool measComplete;
	int64_t rawTemp;
	int64_t rawPres;
	int64_t intPres;
	int64_t intTemp;
	int address;

	bool init(uint8_t addr);	
	bool measure();
	void pressureAltitude();
};