#include "LowerCutDown.h"

void LowerCutDown::init(GPOinterface *wire, BatterySense *battery) {
		this->wire = wire;
		this->battery = battery;

		state = S_MONITORING;
		aboveArmAlt = false;
		aboveCutAlt = false;

		timer = 0;
	}

	// Lower train cutdown sequence:
	// Arm cutdown if:
	//		altitude > 10000ft for more than 20s
	// Initiate cutdown if:
	// 		Altitude exceeds 70k ft for more than 20s

	void LowerCutDown::update() {
		switch(state) {
			default:
			case S_MONITORING: // Waiting for conditions to arm
				if (aboveArmAlt) {
					if (timer >= 20*1000) {
						state = S_ARMED;
						timer = 0;
					}
				}
				else timer = 0;
				break;
			case S_ARMED:  // Waiting for conditions to execute
				if (aboveCutAlt) {
					if (timer >= 20*1000) executeNow();
				}
				else timer = 0;
				break;
			case S_EXECUTING:
				if (timer >= 10*1000) {
					state = S_COMPLETE;
					wire->off();
				}
			case S_COMPLETE:
				break;
		}
	}

	void LowerCutDown::executeNow() {
		state = S_EXECUTING;
		wire->on(1.0, battery->voltage);
		timer = 0;
	}