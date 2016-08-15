#ifndef WS_CONTROLLER_H
#define WS_CONTROLLER_H

#include <WebSocketsServer.h>
#include "Controller.h"
#include "Stream.h"
#include "Utils.h"

class WsController : public Controller {
public:
    WsController(Stream *stream, Print *debug);
    WsController(Stream *stream);
    virtual void setup();
    virtual void loop();
    void webSocketEvent(WebSocketsServer *_server,  uint8_t& num,
                                                    WStype_t& type,
                                                    uint8_t * payload,
                                                    size_t& lenght);
};

#endif