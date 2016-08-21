#include <Arduino.h>

#include "SerialEventDispatcher.h"

SerialEventDispatcher eventDispatcher(&Serial, &Serial);

void setup() {
    Serial.begin(9600);
    Serial.print("started\n");
}

void loop() {
    eventDispatcher.loop();
}
