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

// Delegates from https://zetta-core.com/index.php?threads/%D0%A1-%D0%9D%D0%B5%D0%B2%D0%B5%D1%80%D0%BE%D1%8F%D1%82%D0%BD%D0%BE-%D0%B1%D1%8B%D1%81%D1%82%D1%80%D1%8B%D0%B5-%D0%B4%D0%B5%D0%BB%D0%B5%D0%B3%D0%B0%D1%82%D1%8B-c.53/
class delegate
{
public:
    delegate()
            : object_ptr(0)
            , stub_ptr(0)
    {}

    template <class T, void (T::*TMethod)(String, String)>
    static delegate* from_method(T* object_ptr)
    {
        delegate* d = new delegate();
        d->object_ptr = object_ptr;
        d->stub_ptr = &method_stub<T, TMethod>;
        return d;
    }

    void operator()(String a1, String a2) const
    {
        return (*stub_ptr)(object_ptr, a1, a2);
    }

private:
    typedef void (*stub_type)(void* object_ptr, String, String);

    void* object_ptr;
    stub_type stub_ptr;

    template <class T, void (T::*TMethod)(String, String)>
    static void method_stub(void* object_ptr, String a1, String a2)
    {
        T* p = static_cast<T*>(object_ptr);
        return (p->*TMethod)(a1, a2);
    }
};

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
    ~EventDispatcher() {
        // TODO: clear _listeners
    }

    void on(char* eventType, delegate* handler) {
        _listeners.contains("foo");
        const char* key = eventType;
        delegate** handlers;
        if (!_listeners.contains(key)) {
            handlers = new delegate*[EVENTS_MAX];
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
            delegate** handlers = _listeners.get(key);
            for (int i = 0; i < EVENTS_MAX; i++) {
                if (handlers[i] == NULL) {
                    return;
                } else {
                    delegate *d = handlers[i];
                    (*d)(eventType, event);
                }
            }
        }
    }

    Map<const char*, delegate**, EVENTS_MAX> _listeners;
};

#endif