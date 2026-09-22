#ifndef OBSERVER_H
#define OBSERVER_H

#include "application/events/EventType.h"

class Observer {
    public:
        virtual ~Observer() = default;
        // Copy message inside update() if it must outlive this synchronous call.
        virtual void update(EventType eventType, const char* message = "") = 0;
};

#endif // OBSERVER_H
