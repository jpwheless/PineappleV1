#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    pinMode(13, OUTPUT);
}

void loop() {
    digitalWriteFast(13, HIGH);
    delay(500);
    digitalWriteFast(13, LOW);
    delay(500);
    Serial.println("Yo.");
}

extern "C" int main(void)
{
#if !defined(ARDUINO)

    // To use Teensy 3.1 without Arduino, simply put your code here.
    // For example:

    pinMode(13, OUTPUT);
    while (1) {
        digitalWriteFast(13, HIGH);
        delay(500);
        digitalWriteFast(13, LOW);
        delay(500);
    }


#else
    // Arduino's main() function just calls setup() and loop()....
    setup();
    while (1) {
        loop();
        yield();
    }
#endif
}