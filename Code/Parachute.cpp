#include "Parachute.h"

void Parachute::init(GPOinterface *wire, BatterySense *battery, UpperCutDown *upperCut) {
	this->wire = wire;
	this->battery = battery;
	this->upperCut = upperCut;

	state = S_MONITORING;
	belowDeployAlt = false;
	belowMaxAlt = false;
	falling = false;

	timer = 0;
}

// Parachute deployment sequence:
// Arm parachute if:
// 		(RoC <= -7 ft/min for more than 10s AND
// 		currentAltitude < maxAltitude - 1000ft for more than 10 seconds)
// Deploy parachute if:
// 		(currentAltitude < 10000ft AND 
// 		RoC <= -20 ft/min for more than 5 seconds)
// 			If lower cutdown is not complete, execute immediately
// 			Wait for lower cutdown to complete before executing

void Parachute::update() {
	switch(state) {
		default:
		case S_MONITORING: // Waiting for conditions to arm
			if (belowMaxAlt && falling) {
				if (timer >= 10*1000)  {
					state = S_ARMED;
					timer = 0;
				}
			}
			else timer = 0;
			break;
		case S_ARMED: // Waiting for conditions to execute
			if (falling && belowDeployAlt) {
				if (timer >= 5*1000) {
					state = S_PREPPING;
					timer = 0;
				}
			}
			else timer = 0;
			break;
		case S_PREPPING: // Waiting for upper/lower cutdown to complete
			if (upperCut->state == S_COMPLETE || timer >= 22*1000) executeNow(); // Okay to deploy parachute
			else if (upperCut->state == S_MONITORING ||  upperCut->state == S_ARMED) upperCut->prepNow(); // If not executing, execute
			break;
		case S_EXECUTING: // Wire is hot, waiting to turn off
			if (timer >= 10*1000) {
				state = S_COMPLETE;
				wire->off();
			}
		case S_COMPLETE:
			break;
	}
}

void Parachute::executeNow() {
	state = S_EXECUTING;
	wire->on(1.0, battery->voltage);
	timer = 0;
}