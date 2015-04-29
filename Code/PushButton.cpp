#include <Arduino.h>
#include "Pushbutton.h"

void PushButton::init(unsigned int pin, bool stateFlip, unsigned int debounceTime) {
	this->pin = pin;
	this->stateFlip = stateFlip;
	this->debounceTime = debounceTime;

	pinMode(pin, INPUT);
	lastState = (stateFlip) ? !digitalRead(pin) : digitalRead(pin);
	state = lastState;
	debouncing = false;
	debounceTimer = 0;
	rose = false;
	fell = false;
	changed = false;
}

bool PushButton::read() {
	changed = false;
	rose = false;
	fell = false;

	stateRead = (stateFlip) ? !digitalRead(pin) : digitalRead(pin);
	if (stateRead != lastStateRead) {
		debouncing = true;
		lastStateRead = stateRead;
		debounceTimer = 0;
	}
	else if (debouncing && debounceTimer >= debounceTime) {
		debouncing = false;
		if (stateRead != state) {
			state = stateRead;
			changed = true;
			if (state) rose = true;
			else fell = true;
		}
	}

	return state;
}