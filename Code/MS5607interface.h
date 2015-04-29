#pragma once

#include <i2c_t3.h>

class MS5607interface {
private:
	unsigned int readType;
	int64_t cal[6];
	elapsedMicros timeSinceConv;

	// Return 0 for no measurement, 1 for temperature, 2 for pressure
	unsigned int read() {
		static bool whichMeas = 0; // If 1, pressure.  If 0, temperature.

		// ADC in MS5607 takes about 9.04ms max to complete comversion at 4096 OSR
		if (timeSinceConv > 9040) {
			Wire.beginTransmission(address);
			Wire.write(MS5607_AREAD);
			Wire.endTransmission();
			Wire.requestFrom(address, 3); // 24 bits total

			uint32_t raw = 0;
			raw |= Wire.read() << 16;
			raw |= Wire.read() << 8;
			raw |= Wire.read();

			Wire.beginTransmission(address);
			if (whichMeas) {
				rawPres = raw;
				Wire.write(MS5607_TCONV);
			}
			else {
				rawTemp = raw;
				Wire.write(MS5607_PCONV);
			}
			Wire.endTransmission();
			whichMeas = !whichMeas;
			timeSinceConv = 0;

	    if (!whichMeas) return 2;
	    else return 1;
		}
		else {
			return 0;
		}
	}

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

	bool init(uint8_t addr) {
		this->address = addr;
		kPa = 0;
		rateOfClimb = 0;
		celsius = 0;
		altitude = 0;

		Wire.beginTransmission(address);
	  Wire.write(MS5607_RST);
	 	if (Wire.endTransmission()) return false;
	 	delayMicroseconds(2500); // Need to wait while device resets

		// Read calibration values
		for (unsigned int i = 1; i <= 6; i++){
			cal[i-1] = 0;

			Wire.beginTransmission(address);
	    Wire.write(MS5607_PREAD | (i << 1));
	    if (Wire.endTransmission()) return false;
	    Wire.requestFrom(address, 2); // 6 2-byte calibration values

	    cal[i-1] |= Wire.read() << 8;
	    cal[i-1] |= Wire.read();
	  }

		Wire.beginTransmission(address);
    Wire.write(MS5607_TCONV);
    if (Wire.endTransmission()) return false;
    timeSinceConv = 0;

    measComplete = false;

    return true;
	}
	
	bool measure() {
		static int64_t dT;
		static int64_t offset;
		static int64_t sensitivity;

		if (measComplete) {
			measComplete = false;
		}

		readType = read();
		switch (readType) {
			default:
			case 0:
				return false;
				break;
			case 1: // Calculate temperature
				dT = rawTemp - (cal[4] << 8);
				intTemp = 2000 + (dT*cal[5] >> 23);

				offset = (cal[1] << 17) + (cal[3]*dT >> 6);
				sensitivity = (cal[0] << 16) + (cal[2]*dT >> 7);

				// Second-order correction for low temperature
				if (intTemp < 204700) {
					int64_t temp2;
					int64_t off2;
					int64_t sens2;

					temp2 = (dT*dT) >> 31;
					int64_t intTempMin2000Sq = (intTemp - 2000)*(intTemp - 2000);
					off2 = 61*intTempMin2000Sq >> 4;
					sens2 = 2*intTempMin2000Sq;

					if (intTemp < -1500) {
						int64_t intTempPlus1500Sq = (intTemp + 1500)*(intTemp + 1500);
						off2 += 15*intTempPlus1500Sq;
						sens2 += 8*intTempPlus1500Sq;
					}

					intTemp -= temp2;
					offset -= off2;
					sensitivity -= sens2;
				}

				return false;
				break;
			case 2:
				// Calculate pressure using temperature correction
				intPres = (((rawPres*sensitivity) >> 21) - offset) >> 15;

				celsius = 0.01*intTemp;
				kPa = 0.001*intPres;

				pressureAltitude();

				measComplete = true;
				return true;
				break;
		}
	}

	void pressureAltitude() {
		static elapsedMillis timeSinceLastMeasure = 0;
		static float lastAltitude = -1;
		const float bTbl_P[] = {101.325,22.6321,5.47489,0.86802,0.11091,0.06694,0.00396};
		const float bTbl_T[] = {288.15, 216.65, 216.65, 228.65, 270.65, 270.65, 214.65};
		const float bTbl_L[] = {-0.0065, 0.0, 0.001, 0.0028, 0.0, -0.0028, -0.002};
		const float bTbl_h[] = {0, 11000, 20000, 32000, 47000, 51000, 71000};
		const float M = 0.0288;
		const float g_0 = 9.81;
		const float R = 8.3145;

		int b = 0;
		while (kPa < bTbl_P[b+1] && b < 6) {
			b++;
		}

		if (bTbl_L[b] == 0.0) {
			altitude = -(R*bTbl_T[b]*(log(kPa/bTbl_P[b]) - (M*g_0*bTbl_h[b])/(R*bTbl_T[b])))/(M*g_0);
		}
		else {
			altitude = (bTbl_L[b]*bTbl_h[b] - bTbl_T[b] + bTbl_T[b]/pow(kPa/bTbl_P[b], (bTbl_L[b]*R)/(M*g_0)))/bTbl_L[b];
		}

		if (lastAltitude > 0) { // lastAltitude initialized at -1, to keep RoC from being erroneously high upon start
			rateOfClimb = 0.75*rateOfClimb + 0.25*(altitude - lastAltitude)/(0.001*float(timeSinceLastMeasure));
			timeSinceLastMeasure = 0;
		}
		lastAltitude = altitude;

		if (maxAlt < altitude) {
			maxAlt = altitude;
			maxAltIncreased = true;
		}
		else maxAltIncreased = false;
	}
};