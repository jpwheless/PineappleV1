#pragma once

class LEDinterface {
private:
	unsigned int pin;

public:
	void init(unsigned int pin);
	void write(unsigned int value);
};