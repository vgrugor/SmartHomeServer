#include <string>
#include <unity.h>
#include "application/events/EventNotifier.h"

class RecordingObserver : public Observer {
    public:
        int updateCount = 0;
        EventType lastEvent = EventType::WIFI_START_CONNECT;
        std::string lastMessage;

        void update(EventType eventType, const char* message) override {
            updateCount++;
            lastEvent = eventType;
            lastMessage = message;
        }
};

void setUp() {}
void tearDown() {}

void test_registered_observer_receives_event_and_message() {
    EventNotifier notifier;
    RecordingObserver observer;

    TEST_ASSERT_TRUE(notifier.addObserver(&observer));
    notifier.notifyObservers(EventType::PARAM_SAVED, "temperature saved");

    TEST_ASSERT_EQUAL_INT(1, observer.updateCount);
    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(EventType::PARAM_SAVED),
        static_cast<int>(observer.lastEvent)
    );
    TEST_ASSERT_EQUAL_STRING("temperature saved", observer.lastMessage.c_str());
}

void test_removed_observer_no_longer_receives_events() {
    EventNotifier notifier;
    RecordingObserver observer;

    TEST_ASSERT_TRUE(notifier.addObserver(&observer));
    TEST_ASSERT_TRUE(notifier.removeObserver(&observer));
    notifier.notifyObservers(EventType::WIFI_CONNECTED);

    TEST_ASSERT_EQUAL_INT(0, observer.updateCount);
    TEST_ASSERT_FALSE(notifier.removeObserver(&observer));
}

void test_multiple_observers_receive_the_same_event() {
    EventNotifier notifier;
    RecordingObserver first;
    RecordingObserver second;

    TEST_ASSERT_TRUE(notifier.addObserver(&first));
    TEST_ASSERT_TRUE(notifier.addObserver(&second));
    notifier.notifyObservers(EventType::WIFI_RECONNECT, "connection lost");

    TEST_ASSERT_EQUAL_INT(1, first.updateCount);
    TEST_ASSERT_EQUAL_INT(1, second.updateCount);
    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(first.lastEvent),
        static_cast<int>(second.lastEvent)
    );
    TEST_ASSERT_EQUAL_STRING(first.lastMessage.c_str(), second.lastMessage.c_str());
}

void test_duplicate_registration_is_rejected() {
    EventNotifier notifier;
    RecordingObserver observer;

    TEST_ASSERT_TRUE(notifier.addObserver(&observer));
    TEST_ASSERT_FALSE(notifier.addObserver(&observer));
    notifier.notifyObservers(EventType::WIFI_TRY_CONNECT);

    TEST_ASSERT_EQUAL_INT(1, observer.updateCount);
}

void test_null_observer_and_message_are_safe() {
    EventNotifier notifier;
    RecordingObserver observer;

    TEST_ASSERT_FALSE(notifier.addObserver(nullptr));
    TEST_ASSERT_FALSE(notifier.removeObserver(nullptr));
    TEST_ASSERT_TRUE(notifier.addObserver(&observer));
    notifier.notifyObservers(EventType::PARAM_SAVED, nullptr);

    TEST_ASSERT_EQUAL_INT(1, observer.updateCount);
    TEST_ASSERT_EQUAL_STRING("", observer.lastMessage.c_str());
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_registered_observer_receives_event_and_message);
    RUN_TEST(test_removed_observer_no_longer_receives_events);
    RUN_TEST(test_multiple_observers_receive_the_same_event);
    RUN_TEST(test_duplicate_registration_is_rejected);
    RUN_TEST(test_null_observer_and_message_are_safe);
    return UNITY_END();
}
