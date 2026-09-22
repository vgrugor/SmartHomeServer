#include "presentation/observers/BuzzerObserver.h"

BuzzerObserver::BuzzerObserver(BuzzerActuator& buzzerActuator)
    : buzzerActuator(buzzerActuator), state(false) {}

void BuzzerObserver::update(EventType eventType, const char* message) {
    switch (eventType) {
        //WIFI events
        case EventType::WIFI_START_CONNECT:
            this->state = true;
            this->buzzerActuator.setState(this->state);
            break;
        case EventType::WIFI_TRY_CONNECT:
            this->state = !this->state;
            this->buzzerActuator.setState(this->state);
            break;
        case EventType::WIFI_CONNECTED:
            this->state = false;
            this->buzzerActuator.setState(this->state);
            break;
        case EventType::WIFI_RECONNECT:
            this->state = true;
            this->buzzerActuator.setState(this->state);
            break;
        default:
            break;
    }
}
