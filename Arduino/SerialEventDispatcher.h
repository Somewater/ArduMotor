#ifndef SERIAL_EVENT_DISPATCHER_H
#define SERIAL_EVENT_DISPATCHER_H

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
        _inCmdBufferLen = MAX_CMD_LEN * 2;
        _outCmdBufferLen = MAX_CMD_LEN * 2;

        _inCmdBuffer = (uint8_t*) malloc(_inCmdBufferLen);
        _inCmdBufferPos = 0;
        _outCmdBuffer = (uint8_t*) malloc(_outCmdBufferLen);
        _outCmdBufferPos = 0;
        _stream = stream;
        _debug = debug;
    }
    ~SerialEventDispatcher() {
        free(_inCmdBuffer);
        free(_outCmdBuffer);
    }

    void loop() {
        uint16_t maxLengthLeft = MAX_CMD_LEN << 3;// max quantity of bytes read per loop
        uint16_t bytesAvailable = _stream->available();

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
            if(readBytes > _inCmdBufferLen)
                readBytes = _inCmdBufferLen;

            uint16_t numBytesRead = _stream->readBytes(_inCmdBuffer + _inCmdBufferPos, readBytes);
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
                _stream->print(", _inCmdBufferPos = ");
                _stream->print(_inCmdBufferPos);
                _stream->print(", buffer = ");
                for (int i = 0; i < _inCmdBufferPos + numBytesRead; i++) {
                    char c = (char) * (_inCmdBuffer + i);
                    _stream->print(c);
                }
                _stream->print("\n");
            #endif

            uint16_t bufEnd = _inCmdBufferPos + numBytesRead;
            _inCmdBufferPos = processInputBuffer(_inCmdBuffer, _inCmdBufferPos, bufEnd);

            #ifdef ESP8266
            yield();
            #endif

            if (_outCmdBufferPos) {
                processOutputBuffer();
                #ifdef ESP8266
                yield();
                #endif
            }

            maxLengthLeft -= numBytesRead;
            bytesAvailable = _stream->available();
        }

        if (_outCmdBufferPos) {
            processOutputBuffer();
        }

        #if SERIAL_EVENT_DISPATCHER_DEBUG
            if (cycle)
                debugPrint("after");
        #endif
    }

    virtual void reply(String eventType, String event) override {
        if (eventType.length() + 2 + event.length() > _outCmdBufferLen - _outCmdBufferPos) {
            if (_debug) _debug->print("Output buffer requires resize");
            uint8_t *temp = (uint8_t*) realloc(_outCmdBuffer, (_outCmdBufferLen * 2) * sizeof(uint8_t));
            if ( temp != NULL ) {
                _outCmdBuffer = temp;
                _outCmdBufferLen = _outCmdBufferLen * 2;
            } else {
                if (_debug) _debug->print("Error allocationg memory");
            }
        }
        eventType.getBytes(_outCmdBuffer + _outCmdBufferPos, _outCmdBufferLen - _outCmdBufferPos);
        _outCmdBufferPos += eventType.length();
        _outCmdBuffer[_outCmdBufferPos++] = DELIMITER;
        event.getBytes(_outCmdBuffer + _outCmdBufferPos, _outCmdBufferLen - _outCmdBufferPos);
        _outCmdBufferPos += event.length();
        _outCmdBuffer[_outCmdBufferPos++] = CMD_DELIMITER;
    }

#if SERIAL_EVENT_DISPATCHER_DEBUG
    void debugPrint(const char * name) {
        _stream->print("[");
        _stream->print(name);
        _stream->print("] buffer (");
        _stream->print(_inCmdBufferPos);
        _stream->print(") = ");
        for (int i = 0; i < _inCmdBufferPos; i++) {
            char c = (char) * (_inCmdBuffer + i);
            _stream->print(c);
        }
        _stream->print(".\n");
    }
#endif

protected:
    Stream * _stream;
    Print *_debug;
    uint8_t * _inCmdBuffer;
    uint16_t _inCmdBufferPos;
    uint16_t _inCmdBufferLen;
    
    uint8_t * _outCmdBuffer;
    uint16_t _outCmdBufferPos;
    uint16_t _outCmdBufferLen;

private:
    inline uint8_t processInputBuffer(uint8_t * buf, uint16_t bufStart, uint16_t bufEnd) {
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
                    if (_debug) {
                        _debug->print("Dispatch eventType=");
                        _debug->print(eventType);
                        _debug->print(", event=<empty>\n");
                    }
                    dispatch(eventType, "");

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
                    if (_debug) {
                        _debug->print("Dispatch eventType=");
                        _debug->print(eventType);
                        _debug->print(", event=");
                        _debug->print(event);
                        _debug->print("\n");
                    }
                    dispatch(eventType, event);

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
            }
            if (!eventTypeRead) {
                len += 1 + eventPos;
                buf[eventTypePos] = DELIMITER;
                for (uint16_t i = 0; i < eventPos; i++) {
                    char c = event[i];
                    buf[i + eventTypePos + 1] = c;
                }
            }
            return len;
        } else {
            return 0;
        }
    }

    inline void processOutputBuffer() {
        for (uint8_t i = 0; i < _outCmdBufferPos; i++) {
            char c = (char) _outCmdBuffer[i];
            _stream->print(c);
        }
        _outCmdBufferPos = 0;
    }
};

#endif
