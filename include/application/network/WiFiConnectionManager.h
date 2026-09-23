#ifndef WIFI_CONNECTION_MANAGER_H
#define WIFI_CONNECTION_MANAGER_H

#include <cstdint>
#include "application/events/EventNotifier.h"
#include "application/network/MonotonicClock.h"
#include "application/network/NetworkConnection.h"

class WiFiConnectionManager {
    private:
        static const uint32_t STATUS_NOTIFICATION_INTERVAL_MS = 1000;
        static const uint32_t RECONNECT_INTERVAL_MS = 10000;

        NetworkConnection& connection;
        MonotonicClock& clock;
        EventNotifier& eventNotifier;
        bool started;
        bool wasConnected;
        uint32_t lastStatusNotificationAt;
        uint32_t lastReconnectAttemptAt;

        static bool hasElapsed(uint32_t now, uint32_t since, uint32_t interval);

    public:
        WiFiConnectionManager(
            NetworkConnection& connection,
            MonotonicClock& clock,
            EventNotifier& eventNotifier
        );
        bool begin();
        void update();
        bool isConnected() const;
};

#endif // WIFI_CONNECTION_MANAGER_H
