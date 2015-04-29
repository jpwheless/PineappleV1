#include "UpperCutDown.h"

void UpperCutDown::init(GPOinterface *wire, BatterySense *battery, LowerCutDown *lowerCut) {
		this->wire = wire;
		this->battery = battery;
		this->lowerCut = lowerCut;

		state = S_MONITORING;
		maxAltIncreased = false;
		outsideBoundaries = false;
		aboveArmAlt = false;
		falling = false;

		cutTimer = 0;
		timerA = 0;
		timerB = 0;
	}

	// Balloon cutdown sequence:
	// Arm cutdown if:
	//		altitude > 10000ft for more than 60s
	// Initiate cutdown if:
	// 		(New max altitude is unset for more than 10mins OR
	// 		Location exceeds boundary box for more than 60s OR
	// 		Backup cutdown timer is exceeded) 
	// 			If lower cutdown is not complete, execute immediately
	// 			Wait for lower cutdown to complete before executing

	void UpperCutDown::update() {
		switch(state) {
			default:
			case S_MONITORING: // Waiting for conditions to arm
				if (aboveArmAlt) {
					if (timerA >= 20*1000) {
						state = S_ARMED;
						timerA = 0;
					}
				}
				else timerA = 0;
				break;
			case S_ARMED:  // Waiting for conditions to execute
				if (outsideBoundaries) {
					if (timerA >= 20*1000) prepNow();
				}
				else timerA = 0;
				if (falling) {
					if (timerB >= 10*1000) prepNow();
				}
				timerB = 0;
				if (cutTimer >= 2*3600*1000) prepNow();
				break;
			case S_PREPPING: // Waiting for lower cutdown to complete
				if (lowerCut->state == S_COMPLETE || timerA >= 11*1000) executeNow(); // Okay to cutdown or timeout
				else if (lowerCut->state == S_MONITORING ||  lowerCut->state == S_ARMED) lowerCut->executeNow(); // If not executing, execute
				break;
			case S_EXECUTING: // Wire is hot, waiting to turn off
				if (timerA >= 10*1000) {
					state = S_COMPLETE;
					wire->off();
				}
			case S_COMPLETE:
				break;
		}
	}

	void UpperCutDown::executeNow() {
		state = S_EXECUTING;
		wire->on(1.0, battery->voltage);
		timerA = 0;
	}

	void UpperCutDown::prepNow() {
		state = S_PREPPING;
		timerA = 0;
	}