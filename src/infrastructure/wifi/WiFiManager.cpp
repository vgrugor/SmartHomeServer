#include "infrastructure/wifi/WiFiManager.h"
#include <Arduino.h>

WiFiManager::WiFiManager(
    const char* ssid, 
    const char* password, 
    const char* ip, 
    const char* gateway, 
    const char* subnet
) : 
    ssid(ssid), 
    password(password), 
    ip(ip), 
    gateway(gateway), 
    subnet(subnet),
    wasConnected(false),
    lastStatusNotificationAt(0),
    lastReconnectAttemptAt(0)
{
}

void WiFiManager::begin() {
    IPAddress ip;
    IPAddress gateway;
    IPAddress subnet;

    ip.fromString(this->ip);
    gateway.fromString(this->gateway);
    subnet.fromString(this->subnet);

    WiFi.mode(WIFI_STA);
    WiFi.config(ip, gateway, subnet);
    WiFi.setAutoReconnect(true);
    WiFi.begin(this->ssid, this->password);

    const unsigned long now = millis();
    this->lastStatusNotificationAt = now;
    this->lastReconnectAttemptAt = now;

    EventNotifier::getInstance().notifyObservers(EventType::WIFI_START_CONNECT);
}

void WiFiManager::update() {
    const bool connected = isConnected();
    const unsigned long now = millis();

    if (connected) {
        if (!this->wasConnected) {
            this->wasConnected = true;
            EventNotifier::getInstance().notifyObservers(EventType::WIFI_CONNECTED);
        }

        return;
    }

    if (this->wasConnected) {
        this->wasConnected = false;
        EventNotifier::getInstance().notifyObservers(EventType::WIFI_RECONNECT);
    }

    if (now - this->lastStatusNotificationAt >= 1000) {
        this->lastStatusNotificationAt = now;
        EventNotifier::getInstance().notifyObservers(EventType::WIFI_TRY_CONNECT);
    }

    if (now - this->lastReconnectAttemptAt >= 10000) {
        this->lastReconnectAttemptAt = now;
        WiFi.reconnect();
    }
}

bool WiFiManager::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

String WiFiManager::getIPAddress() const {
    return WiFi.localIP().toString();
}
