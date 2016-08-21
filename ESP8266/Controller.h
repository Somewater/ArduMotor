#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Stream.h"
#include "EventDispatcher.h"

class Controller {
public:
    Controller(EventDispatcher *eventDispatcher, Print *debug = 0) {
        _eventDispatcher = eventDispatcher;
        _debug = debug;

        delegate *onArrow = delegate::from_method<Controller, &Controller::onArrow>(this);
        delegate *onConnectDisconnect = delegate::from_method<Controller, &Controller::onConnectDisconnect>(this);

        _eventDispatcher->on("arrow", onArrow);
        _eventDispatcher->on("connected", onConnectDisconnect);
        _eventDispatcher->on("disconnected", onConnectDisconnect);
    }
    void setup() {
        if (_debug) _debug->println("Controller started");
    }
    void loop() {

    }
    void onArrow(String eventType, String event) {
        debugPrint("onArrow", eventType, event);
        String md5Payload =  md5(eventType + ":" + event);
        _eventDispatcher->reply("reply", md5Payload);
    }

    void onConnectDisconnect(String eventType, String event) {
        debugPrint("onConnectDisconnect", eventType, event);
        if (eventType.equals("connected"))
            _eventDispatcher->reply("hi", "Hello");
        else
            _eventDispatcher->reply("info", "One of us left");
    }
protected:
    EventDispatcher * _eventDispatcher;
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
            #ifdef ESP8266
                _debug->print(", I'm ESP8266");
            #endif
            _debug->print("\n");
        }
    }
};

#endif