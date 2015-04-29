#include <Arduino.h>
#include "LonePineapple.hpp"

extern "C" int main(void) {
	setup();
	while (1) {
		loop();
		yield();
	}
}