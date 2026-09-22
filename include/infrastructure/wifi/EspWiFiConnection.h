#ifndef ESP_WIFI_CONNECTION_H
#define ESP_WIFI_CONNECTION_H

#include "application/network/NetworkConnection.h"

class EspWiFiConnection : public NetworkConnection {
    private:
        const char* ssid;
        const char* password;
        const char* ip;
        const char* gateway;
        const char* subnet;

    public:
        EspWiFiConnection(
            const char* ssid,
            const char* password,
            const char* ip,
            const char* gateway,
            const char* subnet
        );
        void start() override;
        void reconnect() override;
        bool isConnected() const override;
};

#endif // ESP_WIFI_CONNECTION_H
