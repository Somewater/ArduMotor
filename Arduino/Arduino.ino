#include <Arduino.h>

#include "SerialEventDispatcher.h"
#include "SoftwareSerial.h"
#include "ArduinoController.h"

SoftwareSerial softwareSerial(10, 11);
SerialEventDispatcher eventDispatcher(&softwareSerial);
ArduinoController controller(&eventDispatcher, &Serial);

void setup() {
    Serial.begin(9600);
    Serial.print("started\n");

    softwareSerial.begin(9600);
    controller.setup();
}

void loop() {
    eventDispatcher.loop();
    controller.loop();
}

void fromSoftToSerial() {
    while(softwareSerial.available()) {
        Serial.print('.');
        char c = softwareSerial.read();
        Serial.print(c);
    }
}
