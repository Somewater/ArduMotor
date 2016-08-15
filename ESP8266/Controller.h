#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Stream.h"

class Controller {
public:
    Controller(Stream *stream, Print *debug);
    Controller(Stream *stream);
    virtual void setup();
    virtual void loop();
protected:
    Stream *_stream;
    Print *_debug;
};

#endif