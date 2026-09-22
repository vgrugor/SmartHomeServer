#include "presentation/observers/LedObserver.h"

LedObserver::LedObserver(ExternalLedActuator& externalLedActuator) : externalLedActuator(externalLedActuator) {}

void LedObserver::update(EventType eventType, const char* message) {
    switch (eventType) {
        //WIFI events
        case EventType::WIFI_START_CONNECT: 
            this->externalLedActuator.setState(HIGH);
            
            break;
        case EventType::WIFI_TRY_CONNECT: 
            break;
        case EventType::WIFI_CONNECTED: 
            this->externalLedActuator.setState(LOW);

            break;
        case EventType::WIFI_RECONNECT:
            this->externalLedActuator.setState(HIGH);
            break;
        default:
            break;
    }
}
