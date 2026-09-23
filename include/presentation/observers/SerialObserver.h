#ifndef SERIAL_OBSERVER_H
    #define SERIAL_OBSERVER_H

    #include <Arduino.h>
    #include "application/events/Observer.h"

    class SerialObserver : public Observer {
        public:
            void update(EventType eventType, const char* message = "") override;
    };

#endif // SERIAL_OBSERVER_H
