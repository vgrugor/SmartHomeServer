#include "infrastructure/wifi/EspWiFiConnection.h"
#include <ESP8266WiFi.h>

EspWiFiConnection::EspWiFiConnection(
    const char* ssid,
    const char* password,
    const char* ip,
    const char* gateway,
    const char* subnet
) : ssid(ssid),
    password(password),
    ip(ip),
    gateway(gateway),
    subnet(subnet)
{}

void EspWiFiConnection::start() {
    IPAddress ip;
    IPAddress gateway;
    IPAddress subnet;

    ip.fromString(this->ip);
    gateway.fromString(this->gateway);
    subnet.fromString(this->subnet);

    WiFi.mode(WIFI_STA);
    // Static addressing does not obtain DNS from DHCP. Use the configured
    // gateway as DNS so NTP and Telegram hostnames can resolve.
    WiFi.config(ip, gateway, subnet, gateway);
    WiFi.setAutoReconnect(true);
    WiFi.begin(this->ssid, this->password);
}

void EspWiFiConnection::reconnect() {
    WiFi.reconnect();
}

bool EspWiFiConnection::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}
