#include "application/network/WiFiConnectionManager.h"

WiFiConnectionManager::WiFiConnectionManager(
    NetworkConnection& connection,
    MonotonicClock& clock,
    EventNotifier& eventNotifier
) : connection(connection),
    clock(clock),
    eventNotifier(eventNotifier),
    started(false),
    wasConnected(false),
    lastStatusNotificationAt(0),
    lastReconnectAttemptAt(0)
{}

bool WiFiConnectionManager::begin() {
    if (this->started) {
        return false;
    }

    const uint32_t now = this->clock.now();
    this->lastStatusNotificationAt = now;
    this->lastReconnectAttemptAt = now;
    this->started = true;
    this->connection.start();
    this->eventNotifier.notifyObservers(EventType::WIFI_START_CONNECT);
    return true;
}

void WiFiConnectionManager::update() {
    if (!this->started) {
        return;
    }

    const uint32_t now = this->clock.now();

    if (this->connection.isConnected()) {
        if (!this->wasConnected) {
            this->wasConnected = true;
            this->eventNotifier.notifyObservers(EventType::WIFI_CONNECTED);
        }

        return;
    }

    if (this->wasConnected) {
        this->wasConnected = false;
        this->lastStatusNotificationAt = now;
        this->lastReconnectAttemptAt = now;
        this->eventNotifier.notifyObservers(EventType::WIFI_RECONNECT);
        this->connection.reconnect();
        return;
    }

    if (hasElapsed(now, this->lastStatusNotificationAt, STATUS_NOTIFICATION_INTERVAL_MS)) {
        this->lastStatusNotificationAt = now;
        this->eventNotifier.notifyObservers(EventType::WIFI_TRY_CONNECT);
    }

    if (hasElapsed(now, this->lastReconnectAttemptAt, RECONNECT_INTERVAL_MS)) {
        this->lastReconnectAttemptAt = now;
        this->connection.reconnect();
    }
}

bool WiFiConnectionManager::isConnected() const {
    return this->connection.isConnected();
}

bool WiFiConnectionManager::hasElapsed(
    uint32_t now,
    uint32_t since,
    uint32_t interval
) {
    return static_cast<uint32_t>(now - since) >= interval;
}
