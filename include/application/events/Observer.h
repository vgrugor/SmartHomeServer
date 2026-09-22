#ifndef OBSERVER_H
#define OBSERVER_H

#include <Arduino.h>
#include "application/events/EventType.h"

class Observer {
    public:
        virtual ~Observer() = default;
        virtual void update(EventType eventType, const String& message = "") = 0;
};

#endif // OBSERVER_H
