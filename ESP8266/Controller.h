#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Stream.h"
#include "EventDispatcher.h"

class Controller {
public:
    Controller(EventDispatcher *server, EventDispatcher *arduino, Print *debug = 0) {
        _server = server;
        _arduino = arduino;
        _debug = debug;
    }
    void setup() {
        delegate *onArrow = delegate::from_method<Controller, &Controller::onArrow>(this);
        delegate *onConnectDisconnect = delegate::from_method<Controller, &Controller::onConnectDisconnect>(this);
        delegate *onArduinoDebugMsg = delegate::from_method<Controller, &Controller::onArduinoDebugMsg>(this);

        _server->on("arrow", onArrow);
        _server->on("connected", onConnectDisconnect);
        _server->on("disconnected", onConnectDisconnect);
        _arduino->on("debug", onArduinoDebugMsg);

        if (_debug) _debug->println("Controller started");
    }
    void loop() {

    }
    void onArrow(String eventType, String event) {
        debugPrint("onArrow", eventType, event);
        _arduino->reply(eventType, event);
    }

    void onConnectDisconnect(String eventType, String event) {
        debugPrint("onConnectDisconnect", eventType, event);
        if (eventType.equals("connected"))
            _server->reply("hi", "Hello");
        else
            _server->reply("info", "One of us left");
    }

    void onArduinoDebugMsg(String eventType, String event) {
        _server->reply(eventType, event);
    }
protected:
    EventDispatcher * _server; // channel with mobile app through wifi
    EventDispatcher * _arduino;// channel with Arduino
    Print *_debug;

private:
    void debugPrint(const char * from, String eventType, String event) {
        if (_debug) {
            _debug->print("[");
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