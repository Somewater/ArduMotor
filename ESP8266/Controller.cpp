#include "WsController.h"

Controller::Controller(Stream *stream, Print *debug) {
    _stream = stream;
    _debug = debug;
}

Controller::Controller(Stream *stream) {
    Controller(stream, 0);
}

void Controller::setup() {
    if (_debug) _debug->println("Controller started");
}

void Controller::loop() {

}