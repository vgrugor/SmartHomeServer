#include <cstdint>
#include <limits>
#include <vector>
#include <unity.h>
#include "application/network/WiFiConnectionManager.h"

class FakeClock : public MonotonicClock {
    public:
        explicit FakeClock(uint32_t initial = 0) : current(initial) {}

        uint32_t now() const override {
            return current;
        }

        void advance(uint32_t milliseconds) {
            current += milliseconds;
        }

    private:
        uint32_t current;
};

class FakeNetworkConnection : public NetworkConnection {
    public:
        int startCount = 0;
        int reconnectCount = 0;
        bool connected = false;

        void start() override {
            startCount++;
        }

        void reconnect() override {
            reconnectCount++;
        }

        bool isConnected() const override {
            return connected;
        }
};

class WiFiEventRecorder : public Observer {
    public:
        std::vector<EventType> events;

        void update(EventType eventType, const char* message) override {
            events.push_back(eventType);
        }
};

void setUp() {}
void tearDown() {}

void test_begin_starts_connection_once_and_emits_start_event() {
    FakeClock clock;
    FakeNetworkConnection connection;
    EventNotifier notifier;
    WiFiEventRecorder recorder;
    notifier.addObserver(&recorder);
    WiFiConnectionManager manager(connection, clock, notifier);

    TEST_ASSERT_TRUE(manager.begin());
    TEST_ASSERT_FALSE(manager.begin());

    TEST_ASSERT_EQUAL_INT(1, connection.startCount);
    TEST_ASSERT_EQUAL_UINT32(1, recorder.events.size());
    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(EventType::WIFI_START_CONNECT),
        static_cast<int>(recorder.events[0])
    );
}

void test_disconnected_to_connected_emits_connected_once() {
    FakeClock clock;
    FakeNetworkConnection connection;
    EventNotifier notifier;
    WiFiEventRecorder recorder;
    notifier.addObserver(&recorder);
    WiFiConnectionManager manager(connection, clock, notifier);
    manager.begin();

    connection.connected = true;
    manager.update();
    manager.update();

    TEST_ASSERT_TRUE(manager.isConnected());
    TEST_ASSERT_EQUAL_UINT32(2, recorder.events.size());
    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(EventType::WIFI_CONNECTED),
        static_cast<int>(recorder.events[1])
    );
}

void test_connection_loss_emits_reconnect_and_retries_immediately() {
    FakeClock clock;
    FakeNetworkConnection connection;
    EventNotifier notifier;
    WiFiEventRecorder recorder;
    notifier.addObserver(&recorder);
    WiFiConnectionManager manager(connection, clock, notifier);
    manager.begin();

    connection.connected = true;
    manager.update();
    connection.connected = false;
    manager.update();

    TEST_ASSERT_FALSE(manager.isConnected());
    TEST_ASSERT_EQUAL_INT(1, connection.reconnectCount);
    TEST_ASSERT_EQUAL_UINT32(3, recorder.events.size());
    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(EventType::WIFI_RECONNECT),
        static_cast<int>(recorder.events[2])
    );
}

void test_disconnected_connection_retries_every_ten_seconds() {
    FakeClock clock;
    FakeNetworkConnection connection;
    EventNotifier notifier;
    WiFiConnectionManager manager(connection, clock, notifier);
    manager.begin();

    clock.advance(9999);
    manager.update();
    TEST_ASSERT_EQUAL_INT(0, connection.reconnectCount);

    clock.advance(1);
    manager.update();
    TEST_ASSERT_EQUAL_INT(1, connection.reconnectCount);

    clock.advance(9999);
    manager.update();
    TEST_ASSERT_EQUAL_INT(1, connection.reconnectCount);

    clock.advance(1);
    manager.update();
    TEST_ASSERT_EQUAL_INT(2, connection.reconnectCount);
}

void test_emits_status_events_in_connection_lifecycle_order() {
    FakeClock clock;
    FakeNetworkConnection connection;
    EventNotifier notifier;
    WiFiEventRecorder recorder;
    notifier.addObserver(&recorder);
    WiFiConnectionManager manager(connection, clock, notifier);

    manager.begin();
    clock.advance(1000);
    manager.update();
    connection.connected = true;
    manager.update();
    connection.connected = false;
    manager.update();

    TEST_ASSERT_EQUAL_UINT32(4, recorder.events.size());
    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(EventType::WIFI_START_CONNECT),
        static_cast<int>(recorder.events[0])
    );
    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(EventType::WIFI_TRY_CONNECT),
        static_cast<int>(recorder.events[1])
    );
    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(EventType::WIFI_CONNECTED),
        static_cast<int>(recorder.events[2])
    );
    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(EventType::WIFI_RECONNECT),
        static_cast<int>(recorder.events[3])
    );
}

void test_timers_work_across_millis_overflow() {
    const uint32_t initial = std::numeric_limits<uint32_t>::max() - 500;
    FakeClock clock(initial);
    FakeNetworkConnection connection;
    EventNotifier notifier;
    WiFiEventRecorder recorder;
    notifier.addObserver(&recorder);
    WiFiConnectionManager manager(connection, clock, notifier);
    manager.begin();

    clock.advance(1000);
    manager.update();
    TEST_ASSERT_EQUAL_UINT32(2, recorder.events.size());
    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(EventType::WIFI_TRY_CONNECT),
        static_cast<int>(recorder.events[1])
    );

    clock.advance(9000);
    manager.update();
    TEST_ASSERT_EQUAL_INT(1, connection.reconnectCount);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_begin_starts_connection_once_and_emits_start_event);
    RUN_TEST(test_disconnected_to_connected_emits_connected_once);
    RUN_TEST(test_connection_loss_emits_reconnect_and_retries_immediately);
    RUN_TEST(test_disconnected_connection_retries_every_ten_seconds);
    RUN_TEST(test_emits_status_events_in_connection_lifecycle_order);
    RUN_TEST(test_timers_work_across_millis_overflow);
    return UNITY_END();
}
