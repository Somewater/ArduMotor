#ifndef WS_EVENT_DISPATCHER_H
#define WS_EVENT_DISPATCHER_H

#include "EventDispatcher.h"
#include "Stream.h"
#include "Arduino.h"

#define SERIAL_EVENT_DISPATCHER_DEBUG 0

class SerialEventDispatcher : public EventDispatcher
{
public:
    static const char CMD_DELIMITER = '\n';
    static const char ALTER_CMD_DELIMITER = ';';
    static const char DELIMITER = ':'; // between cmd type and payload
    static const uint16_t MAX_CMD_LEN = 128;

    SerialEventDispatcher(Stream *stream, Print *debug = 0) : EventDispatcher() {
        _cmdBufferLen = MAX_CMD_LEN * 2;
        _cmdBuffer = (uint8_t*) malloc(_cmdBufferLen);
        _cmdBufferPos = 0;
        _stream = stream;
        _debug = debug;
    }
    ~SerialEventDispatcher() {
        free(_cmdBuffer);
    }

    void loop() {
        uint16_t maxLengthLeft = MAX_CMD_LEN << 3;// max quantity of bytes read per loop
        uint16_t bytesAvailable = Serial.available();

        #if SERIAL_EVENT_DISPATCHER_DEBUG
        bool cycle = false;
        if ((bytesAvailable > 0) && (maxLengthLeft > 0)) {
            debugPrint("before");
            _stream->print("[Loop] ");
        }
        #endif

        while((bytesAvailable > 0) && (maxLengthLeft > 0)) {
            #if SERIAL_EVENT_DISPATCHER_DEBUG
            cycle = true;
            #endif
            uint16_t readBytes = bytesAvailable;
            if(readBytes > maxLengthLeft)
                readBytes = maxLengthLeft;
            if(readBytes > _cmdBufferLen)
                readBytes = _cmdBufferLen;

            uint16_t numBytesRead = Serial.readBytes(_cmdBuffer + _cmdBufferPos, readBytes);
            if(numBytesRead < 1) {
                #ifdef ESP8266
                yield();
                #endif
                continue;
            }

            #if SERIAL_EVENT_DISPATCHER_DEBUG
            _stream->print("bytesAvailable = ");
            _stream->print(bytesAvailable);
            _stream->print(", maxLengthLeft = ");
            _stream->print(maxLengthLeft);
            _stream->print(", _cmdBufferPos = ");
            _stream->print(_cmdBufferPos);
            _stream->print(", buffer = ");
            for (int i = 0; i < _cmdBufferPos + numBytesRead; i++) {
                char c = (char) * (_cmdBuffer + i);
                _stream->print(c);
            }
            _stream->print("\n");
            #endif

            uint16_t bufEnd = _cmdBufferPos + numBytesRead;
            _cmdBufferPos = processBuffer(_cmdBuffer, _cmdBufferPos, bufEnd);

            #ifdef ESP8266
            yield();
            #endif
            maxLengthLeft -= numBytesRead;
            bytesAvailable = Serial.available();
        }

        #if SERIAL_EVENT_DISPATCHER_DEBUG
        if (cycle)
            debugPrint("after");
        #endif
    }

    virtual void reply(String eventType, String event) override {

    }

#if SERIAL_EVENT_DISPATCHER_DEBUG
    void debugPrint(const char * name) {
        _stream->print("[");
        _stream->print(name);
        _stream->print("] buffer (");
        _stream->print(_cmdBufferPos);
        _stream->print(") = ");
        for (int i = 0; i < _cmdBufferPos; i++) {
            char c = (char) * (_cmdBuffer + i);
            _stream->print(c);
        }
        _stream->print(".\n");
    }
#endif

protected:
    Stream * _stream;
    Print *_debug;
    uint8_t * _cmdBuffer;
    uint16_t _cmdBufferPos;
    uint16_t _cmdBufferLen;

private:
    inline uint8_t processBuffer(uint8_t * buf, uint16_t bufStart, uint16_t bufEnd) {
        char eventType[MAX_CMD_LEN];
        uint16_t eventTypePos = 0;
        char event[MAX_CMD_LEN];
        uint16_t eventPos = 0;
        bool eventTypeRead = true;

        // parse events from buffer
        for (uint16_t i = 0; i < bufEnd; i++) {
            char c = buf[i];
            if (eventTypeRead) {
                if (c == DELIMITER) {
                    eventType[eventTypePos] = '\0';
                    eventTypeRead = false;
                } else if (c == CMD_DELIMITER || c == ALTER_CMD_DELIMITER) {
                    // DISPATCH EVENT (without payload)
                    eventType[eventTypePos] = '\0';
                    dispatch(eventType, "");
                    if (_debug) {
                        _stream->print("Dispatch eventType=");
                        _stream->print(eventType);
                        _stream->print(", event=<empty>\n");
                    }

                    eventTypePos = 0;
                    eventPos = 0;
                    // DISPATCH EVENT END
                    eventTypeRead = true;
                } else {
                    eventType[eventTypePos++] = c;
                }
            } else {
                if (c == CMD_DELIMITER || c == ALTER_CMD_DELIMITER) {
                    event[eventPos] = '\0';
                    // DISPATCH EVENT
                    dispatch(eventType, event);
                    if (_debug) {
                        _stream->print("Dispatch eventType=");
                        _stream->print(eventType);
                        _stream->print(", event=");
                        _stream->print(event);
                        _stream->print("\n");
                    }

                    eventTypePos = 0;
                    eventPos = 0;
                    // DISPATCH EVENT END
                    eventTypeRead = true;
                } else {
                    event[eventPos++] = c;
                }
            }
        }

        // write incomplete event to buffer
        if (eventTypePos || eventPos) {
            uint16_t len = eventTypePos;
            for (uint16_t i = 0; i < eventTypePos; i++) {
                char c = eventType[i];
                buf[i] = c;
//                _stream->print("buf[");
//                _stream->print(i);
//                _stream->print("] = ");hello
//                _stream->print(c);
//                _stream->print("\n");
            }
            if (!eventTypeRead) {
                len += 1 + eventPos;
                buf[eventTypePos] = DELIMITER;
                for (uint16_t i = 0; i < eventPos; i++) {
                    char c = event[i];
                    buf[i + eventTypePos + 1] = c;
//                    _stream->print("buf[");
//                    _stream->print(i + eventTypePos + 1);
//                    _stream->print("] = ");
//                    _stream->print(c);
//                    _stream->print("\n");
                }
            }
            return len;
        } else {
            return 0;
        }
    }
};

#endif