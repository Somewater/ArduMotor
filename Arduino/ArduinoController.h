#ifndef ARDUINO_CONTROLLER_H
#define ARDUINO_CONTROLLER_H

#include "Arduino.h"
#include "EventDispatcher.h"

class ArduinoController {
public:
    ArduinoController(EventDispatcher * server, Print * debug = 0) {
        _server = server;
        _debug = debug;
        unsigned long now = millis();
        every1sec = now;
    }

    void setup() {
        delegate *onArrow = delegate::from_method<ArduinoController, &ArduinoController::onArrow>(this);
        delegate *onServerDebug = delegate::from_method<ArduinoController, &ArduinoController::onServerDebug>(this);

        _server->on("arrow", onArrow);
        _server->on("debug", onServerDebug);

        pinMode(2, OUTPUT);
        pinMode(3, OUTPUT);
        pinMode(4, OUTPUT);
        pinMode(5, OUTPUT);

        if (_debug) _debug->print("ArduinoController started\n");
    }

    void loop() {
        unsigned long now = millis();
        if (now - every1sec > 1000) {
            onEvery1Sec(now);
            every1sec = now;
        }
    }

protected:

    void onEvery1Sec(unsigned long now) {
        //_server->reply("debug", "arduino_is_alive:" + now);
    }

    void onArrow(String eventType, String event) {
        debugPrint("onArrow", eventType, event);
        int delimIdx = event.indexOf(":");
        String button = event.substring(0, delimIdx);
        String mode = event.substring(delimIdx + 1);

        int pin = 0;
        if (button.equals("left")) {
            pin = 2;
        } else if (button.equals("up")) {
            pin = 3;
        } else if (button.equals("right")) {
            pin = 4;
        } else if (button.equals("down")) {
            pin = 5;
        }
        if (pin) {
            if (mode.equals("down"))
                digitalWrite(pin, HIGH);
            else
                digitalWrite(pin, LOW);
        }
    }

    void onServerDebug(String eventType, String event) {
        debugPrint("onServerDebug", eventType, event);
    }

private:
    EventDispatcher * _server;
    Print * _debug;
    unsigned long every1sec;

    void debugPrint(const char * from, String eventType, String event) {
        if (_debug) {
            _debug->print("[ArduinoController::");
            _debug->print(from);
            _debug->print("] invoked, eventType=");
            _debug->print(eventType);
            _debug->print(", event=");
            _debug->print(event);
            _debug->print("\n");
        }
    }
};

#endif