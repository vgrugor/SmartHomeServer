#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <ESP8266WiFi.h>
#include "application/events/EventNotifier.h"

class WiFiManager {
    private:
        const char* ssid;
        const char* password;
        const char* ip;
        const char* gateway;
        const char* subnet;
        bool wasConnected;
        unsigned long lastStatusNotificationAt;
        unsigned long lastReconnectAttemptAt;

    public:
        WiFiManager(const char* ssid, const char* password, const char* ip, const char* gateway, const char* subnet);
        void begin();
        void update();
        bool isConnected() const;
        String getIPAddress() const;
};

#endif // WIFI_MANAGER_H
