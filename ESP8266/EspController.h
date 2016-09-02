#ifndef ESP_CONTROLLER_H
#define ESP_CONTROLLER_H

#include "Stream.h"
#include "EventDispatcher.h"

class EspController {
public:
    EspController(EventDispatcher *server, EventDispatcher *arduino, Print *debug = 0) {
        _server = server;
        _arduino = arduino;
        _debug = debug;
    }
    void setup() {
        delegate *onArrow = delegate::from_method<EspController, &EspController::onArrow>(this);
        delegate *onConnectDisconnect = delegate::from_method<EspController, &EspController::onConnectDisconnect>(this);
        delegate *onArduinoDebugMsg = delegate::from_method<EspController, &EspController::onArduinoDebugMsg>(this);
        delegate *onArduinoHi = delegate::from_method<EspController, &EspController::onArduinoHi>(this);
        delegate *onPing = delegate::from_method<EspController, &EspController::onPing>(this);
        delegate *onArduinoPong = delegate::from_method<EspController, &EspController::onArduinoPong>(this);

        _server->on("arrow", onArrow);
        _server->on("connected", onConnectDisconnect);
        _server->on("disconnected", onConnectDisconnect);
        _server->on("ping", onPing);

        _arduino->on("debug", onArduinoDebugMsg);
        _arduino->on("hi", onArduinoHi);
        _arduino->on("pong", onArduinoPong);

        if (_debug) _debug->println("EspController started");
    }
    void loop() {

    }
    void onArrow(String eventType, String event) {
        debugPrint("onArrow", eventType, event);
        _arduino->reply(eventType, event);
    }

    void onConnectDisconnect(String eventType, String event) {
        debugPrint("onConnectDisconnect", eventType, event);
        if (eventType.equals("connected")) {
            _server->reply("hi", event);
            _arduino->reply("connected", event);
        } else {
            _server->reply("disconnected", event);
            _arduino->reply("disconnected", event);
        }
    }

    void onArduinoDebugMsg(String eventType, String event) {
        _server->reply("debug_arduino", event);
    }

    void onArduinoHi(String eventType, String event) {
        _server->reply("hi_arduino", event);
    }

    void onPing(String eventType, String event) {
        _server->reply("pong", event);
        _arduino->reply(eventType, event);
    }

    void onArduinoPong(String eventType, String event) {
        _server->reply("pong_arduino", event);
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

    void debug(String msg, bool toApp) {
        if (_debug) _debug->println("[DEBUG] " + msg);
        if (toApp)
            _server->reply("debug", msg);
        else
            _arduino->reply("debug", msg);
    }
};

#endif