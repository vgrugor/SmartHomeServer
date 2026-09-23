#ifndef LED_OBSERVER_H
    #define LED_OBSERVER_H

    #include <Arduino.h>
    #include "application/events/Observer.h"
    #include "infrastructure/actuators/ExternalLedActuator.h"

    class LedObserver : public Observer {
        private:
            ExternalLedActuator& externalLedActuator;

        public:
            LedObserver(ExternalLedActuator& externalLedActuator);
            void update(EventType eventType, const char* message = "") override;
    };

#endif // LED_OBSERVER_H
