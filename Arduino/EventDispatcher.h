#ifndef EVENT_DISPATCHER_H
#define EVENT_DISPATCHER_H

//#include "WString.h"
#include "Vector.h"
#include "HashMap.h"

#ifndef EVENTS_MAX
    #define EVENTS_MAX 10
#endif

#ifndef EVENT_TYPE_MAX
    #define EVENT_TYPE_MAX 30
#endif

#ifndef EVENT_HANDLERS_MAX
    #define EVENT_HANDLERS_MAX 3
#endif

typedef void (*EventHandler)(String eventType, String event);

bool compareKeys(const char * k1, const char * k2) {
    for (int i = 0; i < EVENT_TYPE_MAX; i ++) {
        char c1 = *(k1 + i);
        char c2 = *(k2 + i);
        if (c1 != c2) {
            return false;
        } else if (c1 == '\0' || c2 == '\0') {
            return true;
        }
    }
    return true;
}

class EventDispatcher
{
public:
    EventDispatcher() : _listeners(compareKeys) {

    }
    void on(char* eventType, EventHandler handler) {
        _listeners.contains("foo");
        const char* key = eventType;
        EventHandler* handlers;
        if (!_listeners.contains(key)) {
            handlers = new EventHandler[EVENTS_MAX];
            for (int i = 0; i < EVENTS_MAX; i++) {
                handlers[i] = NULL;
            }
            _listeners.set(key, handlers);
        } else {
            handlers = _listeners.get(key);
        }

        for (int i = 0; i < EVENTS_MAX; i++) {
            if (handlers[i] == NULL) {
                handlers[i] = handler;
                return;
            }
        }
        return;
    }

    virtual void reply(String eventType, String event){}

//protected:
    void dispatch(String eventType, String event) {
        const char * key = eventType.c_str();
        if (_listeners.contains(key)) {
            EventHandler* handlers = _listeners.get(key);
            for (int i = 0; i < EVENTS_MAX; i++) {
                if (handlers[i] == NULL) {
                    return;
                } else {
                    handlers[i](eventType, event);
                }
            }
        }
    }

    Map<const char*, EventHandler*, EVENTS_MAX> _listeners;
};

#endif