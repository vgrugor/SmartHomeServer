#ifndef BUZZER_OBSERVER_H
    #define BUZZER_OBSERVER_H

    #include <Arduino.h>
    #include "application/events/Observer.h"
    #include "infrastructure/actuators/BuzzerActuator.h"

    class BuzzerObserver : public Observer {
        private:
            BuzzerActuator& buzzerActuator;
            bool state;

        public:
            BuzzerObserver(BuzzerActuator& buzzerActuator);
            void update(EventType eventType, const char* message = "") override;
    };

#endif // BUZZER_OBSERVER_H
