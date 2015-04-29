#pragma once

class PushButton {
private:
	unsigned int debounceTime;
	unsigned int pin;
	bool stateFlip; // True if button true is electrical low

	elapsedMicros debounceTimer;
	bool lastStateRead;
	bool debouncing;
	bool stateRead;
	bool lastState;

public:
	bool rose, fell, changed; // Reset upon next read
	bool state;
	
	void init(unsigned int pin, bool stateFlip, unsigned int debounceTime);
	bool read();
};