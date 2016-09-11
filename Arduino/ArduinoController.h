#ifndef ARDUINO_CONTROLLER_H
#define ARDUINO_CONTROLLER_H

#include "Arduino.h"
#include "EventDispatcher.h"

#define ARROW_LEFT  0
#define ARROW_UP    1
#define ARROW_RIGHT 2
#define ARROW_DOWN  3

#define FL 0
#define FR 1
#define BL 2
#define BR 3

#define SPEED 255

struct ArduinoMotorPins {
    int shiftInput1;
    int shiftInput2;
    int enable;
};

struct ArduinoPins {
    int espRx;
    int espTx;
    int shiftData;
    int shiftLatch;
    int shiftClock;
    ArduinoMotorPins fl;
    ArduinoMotorPins fr;
    ArduinoMotorPins bl;
    ArduinoMotorPins br;
};

class ArduinoController {
public:
    ArduinoController(EventDispatcher * server, ArduinoPins * pins, Print * debug = 0) {
        _server = server;
        _debug = debug;
        unsigned long now = millis();
        every1sec = now;
        _silent = false;
        _pins = pins;
        shiftPins = 0;
        motorsSpeed[4];
        for(int i = 0; i < 4; i++) motorsSpeed[i] = 0;
        arrows[4];
        for(int i = 0; i < 4; i++) arrows[i] = 0;
    }

    void setup() {
        delegate *onArrow = delegate::from_method<ArduinoController, &ArduinoController::onArrow>(this);
        delegate *onServerDebug = delegate::from_method<ArduinoController, &ArduinoController::onServerDebug>(this);
        delegate *onConnected = delegate::from_method<ArduinoController, &ArduinoController::onConnected>(this);
        delegate *onDisconnected = delegate::from_method<ArduinoController, &ArduinoController::onDisconnected>(this);
        delegate *onPing = delegate::from_method<ArduinoController, &ArduinoController::onPing>(this);

        _server->on("arrow", onArrow);
        _server->on("debug", onServerDebug);
        _server->on("connected", onConnected);
        _server->on("connected", onDisconnected);
        _server->on("ping", onPing);

        pinMode(_pins->shiftData, OUTPUT);
        pinMode(_pins->shiftLatch, OUTPUT);
        pinMode(_pins->shiftClock, OUTPUT);
        pinMode(_pins->fl.enable, OUTPUT);
        pinMode(_pins->fr.enable, OUTPUT);
        pinMode(_pins->bl.enable, OUTPUT);
        pinMode(_pins->br.enable, OUTPUT);

        if (_debug) _debug->print("ArduinoController started\n");
    }

    void loop() {
        unsigned long now = millis();
        if (now - every1sec > 1000) {
            onEvery1Sec(now);
            //refreshMotors();
            every1sec = now;
        }
        refreshMotors();
    }

    void debug(String msg) {
        if (_debug)
            _debug->print(msg + "\n");
        if(!_silent)
            _server->reply("debug", msg);
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

        uint8_t arrowIndex = 5;
        if (button.equals("left")) {
            arrowIndex = ARROW_LEFT;
        } else if (button.equals("up")) {
            arrowIndex = ARROW_UP;
        } else if (button.equals("right")) {
            arrowIndex = ARROW_RIGHT;
        } else if (button.equals("down")) {
            arrowIndex = ARROW_DOWN;
        }
        if (arrowIndex != 5) {
            if (mode.equals("down"))
                arrows[arrowIndex] = 1;
            else
                arrows[arrowIndex] = 0;

            arrowsToMotorState();
        }
    }

    void onServerDebug(String eventType, String event) {
        debugPrint("onServerDebug", eventType, event);
    }

    void onConnected(String eventType, String event) {
        if (!_silent)
            _server->reply("hi", event);
    }

    void onDisconnected(String eventType, String event) {

    }

    void onPing(String eventType, String event) {
        if (!_silent)
            _server->reply("pong", event);
    }

private:
    EventDispatcher * _server;
    Print * _debug;
    unsigned long every1sec;
    bool _silent;
    ArduinoPins * _pins;
    uint16_t shiftPins;
    int motorsSpeed[4];
    uint8_t arrows[4];


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

    // translate pressed keys to motor states
    void arrowsToMotorState() {
        int8_t pull = 0; // forward (1) or backward (-1)
        int8_t direction = 0;// right (1) or left (-1)

        if (arrows[ARROW_UP] && !arrows[ARROW_DOWN])
            pull = 1;
        else if (arrows[ARROW_DOWN] && !arrows[ARROW_UP])
            pull = -1;

        if (arrows[ARROW_LEFT] && !arrows[ARROW_RIGHT])
            direction = -1;
        else if (arrows[ARROW_RIGHT] && !arrows[ARROW_LEFT])
            direction = 1;

        motorsSpeed[FL] = 0;
        motorsSpeed[FR] = 0;
        motorsSpeed[BL] = 0;
        motorsSpeed[BR] = 0;

        if (pull) {
            if (direction != 1) {
                motorsSpeed[FR] = SPEED * pull;
                motorsSpeed[BR] = SPEED * pull;
            }
            if (direction != -1) {
                motorsSpeed[FL] = SPEED * pull;
                motorsSpeed[BL] = SPEED * pull;
            }
        } else if(direction) {
            motorsSpeed[FR] = SPEED * -direction;
            motorsSpeed[BR] = SPEED * -direction;
            motorsSpeed[FL] = SPEED * direction;
            motorsSpeed[BL] = SPEED * direction;
        }
    }

    void refreshMotors() {
        refreshMotor(&_pins->fl, motorsSpeed[FL], "FL");
        refreshMotor(&_pins->fr, motorsSpeed[FR], "FR");
        refreshMotor(&_pins->bl, motorsSpeed[BL], "BL");
        refreshMotor(&_pins->br, motorsSpeed[BR], "BR");

//        if (_debug) {
//            _debug->print("motorsSpeed={");
//            _debug->print(motorsSpeed[FL]);_debug->print(", ");
//            _debug->print(motorsSpeed[FR]);_debug->print(", ");
//            _debug->print(motorsSpeed[BL]);_debug->print(", ");
//            _debug->print(motorsSpeed[BR]);_debug->print("}\n");
//        }

        flushShiftPins();
    }

    // speed between -255..255
    void refreshMotor(ArduinoMotorPins * pins, int speed, const char * motorName) {
        analogWrite(pins->enable, abs(speed));
        bool forwardSpeed = speed >= 0;
        if (speed) {
            bitWrite(shiftPins, pins->shiftInput1, forwardSpeed);
            bitWrite(shiftPins, pins->shiftInput2, !forwardSpeed);
        } else {
            bitWrite(shiftPins, pins->shiftInput1, 0);
            bitWrite(shiftPins, pins->shiftInput2, 0);
        }

//        if(_debug) {
//            _debug->print(motorName);
//            _debug->print(", speed=");
//            _debug->print(speed);
//            _debug->print(", forwardSpeed=");
//            _debug->print(forwardSpeed);
//            _debug->print(" (enabled=");
//            _debug->print(pins->enable);
//            _debug->print(", inp1=");
//            _debug->print(pins->shiftInput1);
//            _debug->print(", inp2=");
//            _debug->print(pins->shiftInput2);
//            _debug->print(")\n");
//        }
    }

    inline void setShiftPin(uint8_t pin, uint8_t value) {
        bitWrite(shiftPins, pin, value);
    }

    void flushShiftPins() {
//        if (_debug) {
//            _debug->print("shiftPins=");
//            _debug->print(String(shiftPins, 2));
//            _debug->print("\n");
//        }
        digitalWrite(_pins->shiftLatch, LOW);
        shiftOut(_pins->shiftData, _pins->shiftClock, MSBFIRST, shiftPins);
        digitalWrite(_pins->shiftLatch, HIGH);
    }
};

#endif