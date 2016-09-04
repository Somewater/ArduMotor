#include <Arduino.h>

#include "SerialEventDispatcher.h"
#include "SoftwareSerial.h"
#include "ArduinoController.h"

ArduinoPins pins = {
        10, // esp rx
        11, // esp tx
        2,  // data
        4,  // latch
        7,  // clock
        {0, 1, 3},   // FL (shiftIn1, shiftIn2, enable PWM pin)
        {2, 3, 5},   // FR
        {4, 5, 6},   // BL
        {6, 7, 9}    // BR
};

SoftwareSerial softwareSerial(pins.espRx, pins.espTx);
SerialEventDispatcher eventDispatcher(&softwareSerial);
ArduinoController controller(&eventDispatcher, &pins, &Serial);

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
