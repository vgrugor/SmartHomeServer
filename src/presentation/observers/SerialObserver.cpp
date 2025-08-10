#include "presentation/observers/SerialObserver.h"

void SerialObserver::update(EventType eventType, const String& message) {
    String text;

    switch (eventType) {
        //WIFI events
        case EventType::WIFI_START_CONNECT: text = "Start connecting to WiFi"; break;
        case EventType::WIFI_TRY_CONNECT: text = "."; break;
        case EventType::WIFI_CONNECTED: text = "WiFi connected"; break;
        case EventType::WIFI_RECONNECT: text = "WiFi reconnect"; break;
        
        //data events
        case EventType::PARAM_SAVED: text = message; break;
    }

    Serial.println(text);
}
