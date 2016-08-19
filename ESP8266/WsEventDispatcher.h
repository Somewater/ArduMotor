#ifndef WS_EVENT_DISPATCHER_H
#define WS_EVENT_DISPATCHER_H

#include "EventDispatcher.h"
#include <WebSocketsServer.h>

class WsEventDispatcher : public EventDispatcher
{
public:
    WsEventDispatcher(WebSocketsServer *server, Print *debug = 0) : EventDispatcher() {
        _server = server;
        _debug = debug;
    }

    void webSocketEvent(uint8_t& num,
                        WStype_t& type,
                        uint8_t * payload,
                        size_t& lenght) {
        switch(type) {
            case WStype_DISCONNECTED:
                if (_debug)
                    _debug->printf("[%u] Disconnected!\n", num);
                dispatch("disconnected", String(num));
                break;
            case WStype_CONNECTED:
            {
                if (_debug) {
                    IPAddress ip = _server->remoteIP(num);
                    _debug->printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
                }

                // send message to client
                dispatch("connected", String(num));
            }
                break;
            case WStype_TEXT:
                if (_debug)
                    _debug->printf("[%u] get text: %s\n", num, payload);
                handleText(num, type, payload, lenght);
                break;
            case WStype_BIN:
                if (_debug) {
                    _debug->printf("[%u] get binary lenght: %u\n", num, lenght);
                    hexdump(payload, lenght);
                }
                break;
        }
    }

    virtual void reply(String eventType, String event) override {
        _server->broadcastTXT(eventType + ':' + event);
    }

protected:
    WebSocketsServer * _server;
    Print *_debug;

    void handleText(uint8_t& num,
                    WStype_t& type,
                    uint8_t * payload,
                    size_t& lenght) {
        char eventType[lenght + 1];
        int eventTypeLen = 0;
        char event[lenght + 1];
        int eventLen = 0;
        boolean eventTypeMode = true;
        for (int i = 0; i < lenght; i++) {
            char c = *(payload + i);
            if (c == ':' && eventTypeMode) {
                eventType[eventTypeLen] = '\0';
                eventTypeMode = false;
                continue;
            }
            if (eventTypeMode) {
                eventType[eventTypeLen++] = c;
            } else {
                event[eventLen++] = c;
            }
        }
        event[eventLen] = '\0';

        dispatch(eventType, event);
    }
};

#endif