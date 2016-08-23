#include <Arduino.h>

#include "SerialEventDispatcher.h"
#include "SoftwareSerial.h"

SerialEventDispatcher eventDispatcher(&Serial);
SoftwareSerial softwareSerial(10, 11);

void setup() {
    Serial.begin(9600);
    Serial.print("started\n");

    softwareSerial.begin(9600);
}

void loop() {
    while(softwareSerial.available()) {
        Serial.print('.');
        char c = softwareSerial.read();
        Serial.print(c);
    }
}
