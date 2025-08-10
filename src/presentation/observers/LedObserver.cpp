#include "presentation/observers/LedObserver.h"

LedObserver::LedObserver(ExternalLedActuator externalLedActuator) : externalLedActuator(externalLedActuator) {}

void LedObserver::update(EventType eventType, const String& message) {
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
            break;
        
        //temperature events
        case EventType::READ_TEMPERATURE: 
            this->externalLedActuator.setState(HIGH);
            delay(100);
            this->externalLedActuator.setState(LOW);
            delay(100);
            
            break;
        case EventType::SEND_TEMPERATURE:
            this->externalLedActuator.setState(HIGH);
            delay(1000);
            this->externalLedActuator.setState(LOW);
            delay(1000);
            this->externalLedActuator.setState(HIGH);
            delay(1000);
            this->externalLedActuator.setState(LOW);
            delay(1000);

            break;
    }
}
