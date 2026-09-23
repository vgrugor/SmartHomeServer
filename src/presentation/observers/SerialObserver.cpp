#include "presentation/observers/SerialObserver.h"

void SerialObserver::update(EventType eventType, const char* message) {
    switch (eventType) {
        //WIFI events
        case EventType::WIFI_START_CONNECT:
            Serial.println("Start connecting to WiFi");
            break;
        case EventType::WIFI_TRY_CONNECT:
            Serial.println(".");
            break;
        case EventType::WIFI_CONNECTED:
            Serial.println("WiFi connected");
            break;
        case EventType::WIFI_RECONNECT:
            Serial.println("WiFi reconnect");
            break;
        
        //data events
        case EventType::PARAM_SAVED:
            Serial.println(message);
            break;

        default:
            break;
    }
}
