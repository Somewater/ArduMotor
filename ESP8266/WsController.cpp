#include "WsController.h"
#include <WebSocketsServer.h>

WsController::WsController(Stream *stream, Print *debug)
        : Controller(stream, debug) {
}

WsController::WsController(Stream *stream)
        : Controller(stream) {
    WsController(stream, 0);
}

void WsController::setup() {
    Controller::setup();
}

void WsController::loop() {
    Controller::loop();
}

void WsController::webSocketEvent(WebSocketsServer *_server,  uint8_t& num,
                                                              WStype_t& type,
                                                              uint8_t * payload,
                                                              size_t& lenght) {
    String md5Payload;

    switch(type) {
        case WStype_DISCONNECTED:
            if (_debug) _debug->printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED:
        {
            IPAddress ip = _server->remoteIP(num);
            if (_debug) _debug->printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            // send message to client
            _server->sendTXT(num, "Connected");
        }
            break;
        case WStype_TEXT:
            if (_debug) _debug->printf("[%u] get Text: %s\n", num, payload);

            // send message to client
            md5Payload =  md5(payload, lenght);
            _server->sendTXT(num, md5Payload);

            // send data to all connected clients
            // webSocket.broadcastTXT("message here");
            break;
        case WStype_BIN:
            if (_debug) _debug->printf("[%u] get binary lenght: %u\n", num, lenght);
            hexdump(payload, lenght);

            // send message to client
            // webSocket.sendBIN(num, payload, lenght);
            break;
    }
}
