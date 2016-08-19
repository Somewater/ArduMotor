#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Stream.h"
#include "EventDispatcher.h"

class Controller {
public:
    Controller(EventDispatcher *eventDispatcher, Print *debug = 0) {
        _eventDispatcher = eventDispatcher;
        _debug = debug;
    }
    void setup() {
        if (_debug) _debug->println("Controller started");
    }
    void loop() {

    }
    void onEvent(String eventType, String event) {
        String md5Payload =  md5(eventType + ":" + event);
        _eventDispatcher->reply("reply", md5Payload);
    }
protected:
    EventDispatcher * _eventDispatcher;
    Print *_debug;
};

#endif