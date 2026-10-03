#include <cstdint>
#include <limits>
#include <unity.h>
#include "application/reporting/DailyTemperatureReporter.h"

class FakeClock : public MonotonicClock {
    public:
        uint32_t milliseconds = 0;
        uint32_t now() const override { return milliseconds; }
};

class FakeDateTime : public LocalDateTimeSource {
    public:
        bool valid = true;
        LocalDateTime current = {2026, 9, 25, 14, 59};

        bool now(LocalDateTime& result) const override {
            if (!valid) return false;
            result = current;
            return true;
        }
};

class FakeSender : public TemperatureMessageSender {
    public:
        bool succeeds = true;
        int calls = 0;
        int sentDate = 0;
        int sentHour = -1;
        int sentMinute = -1;
        float house = 0;
        float outdoor = 0;

        bool send(int dateKey, int hour, int minute, float houseC, float outdoorC) override {
            calls++;
            sentDate = dateKey;
            sentHour = hour;
            sentMinute = minute;
            house = houseC;
            outdoor = outdoorC;
            return succeeds;
        }
};

class FakeStore : public SentDateStore {
    public:
        bool loads = true;
        bool saves = true;
        int date = 0;
        int saveCalls = 0;

        bool load(int& dateKey) override {
            dateKey = date;
            return loads;
        }

        bool save(int dateKey) override {
            saveCalls++;
            if (saves) date = dateKey;
            return saves;
        }
};

struct Fixture {
    SensorData sensors;
    FakeClock clock;
    FakeDateTime time;
    FakeSender sender;
    FakeStore store;
    DailyTemperatureReporter reporter;

    Fixture() : reporter(sensors, time, clock, sender, store) {
        sensors.setValue(SensorType::HOUSE_TEMP, 21.25f);
        sensors.setValue(SensorType::OUTDOOR_TEMP, -3.5f);
        TEST_ASSERT_TRUE(reporter.begin());
    }
};

void setUp() {}
void tearDown() {}

void test_sends_at_15_once_with_both_temperatures() {
    Fixture f;
    f.reporter.update(true);
    TEST_ASSERT_EQUAL_INT(0, f.sender.calls);

    f.time.current.hour = 15;
    f.time.current.minute = 0;
    f.reporter.update(true);
    f.reporter.update(true);

    TEST_ASSERT_EQUAL_INT(1, f.sender.calls);
    TEST_ASSERT_EQUAL_INT(20260925, f.sender.sentDate);
    TEST_ASSERT_EQUAL_INT(15, f.sender.sentHour);
    TEST_ASSERT_EQUAL_INT(0, f.sender.sentMinute);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 21.25f, f.sender.house);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -3.5f, f.sender.outdoor);
    TEST_ASSERT_EQUAL_INT(20260925, f.store.date);
    TEST_ASSERT_TRUE(f.reporter.isReady());
    TEST_ASSERT_EQUAL_INT(20260925, f.reporter.getLastSentDate());
    TEST_ASSERT_FALSE(f.reporter.hasPendingSave());
}

void test_sends_after_missed_deadline_when_power_returns() {
    Fixture f;
    f.time.current.hour = 18;
    f.reporter.update(false);
    TEST_ASSERT_EQUAL_INT(0, f.sender.calls);

    f.reporter.update(true);
    TEST_ASSERT_EQUAL_INT(1, f.sender.calls);
    TEST_ASSERT_EQUAL_INT(18, f.sender.sentHour);
}

void test_persisted_day_blocks_duplicate_after_restart() {
    Fixture f;
    f.time.current.hour = 15;
    f.reporter.update(true);

    FakeSender secondSender;
    DailyTemperatureReporter afterRestart(
        f.sensors, f.time, f.clock, secondSender, f.store
    );
    TEST_ASSERT_TRUE(afterRestart.begin());
    afterRestart.update(true);
    TEST_ASSERT_EQUAL_INT(0, secondSender.calls);

    f.time.current.day = 26;
    afterRestart.update(true);
    TEST_ASSERT_EQUAL_INT(1, secondSender.calls);
    TEST_ASSERT_EQUAL_INT(20260926, f.store.date);
}

void test_waits_for_valid_local_time_and_both_readings() {
    Fixture f;
    f.time.current.hour = 15;
    f.time.valid = false;
    f.reporter.update(true);
    TEST_ASSERT_EQUAL_INT(0, f.sender.calls);

    f.time.valid = true;
    f.sensors.removeKey(SensorType::OUTDOOR_TEMP);
    f.reporter.update(true);
    TEST_ASSERT_EQUAL_INT(0, f.sender.calls);

    f.sensors.setValue(SensorType::OUTDOOR_TEMP, 4.0f);
    f.reporter.update(true);
    TEST_ASSERT_EQUAL_INT(1, f.sender.calls);
}

void test_failed_send_retries_after_five_minutes() {
    Fixture f;
    f.time.current.hour = 15;
    f.sender.succeeds = false;
    f.reporter.update(true);
    TEST_ASSERT_EQUAL_INT(1, f.sender.calls);
    TEST_ASSERT_EQUAL_INT(0, f.store.saveCalls);

    f.clock.milliseconds = 299999;
    f.reporter.update(true);
    TEST_ASSERT_EQUAL_INT(1, f.sender.calls);

    f.clock.milliseconds = 300000;
    f.sender.succeeds = true;
    f.reporter.update(true);
    TEST_ASSERT_EQUAL_INT(2, f.sender.calls);
    TEST_ASSERT_EQUAL_INT(20260925, f.store.date);
}

void test_failed_persistence_never_resends_during_this_boot() {
    Fixture f;
    f.time.current.hour = 15;
    f.store.saves = false;
    f.reporter.update(true);
    TEST_ASSERT_EQUAL_INT(1, f.sender.calls);
    TEST_ASSERT_EQUAL_INT(1, f.store.saveCalls);
    TEST_ASSERT_TRUE(f.reporter.hasPendingSave());

    f.clock.milliseconds = 300000;
    f.store.saves = true;
    f.reporter.update(true);
    f.reporter.update(true);
    TEST_ASSERT_EQUAL_INT(1, f.sender.calls);
    TEST_ASSERT_EQUAL_INT(2, f.store.saveCalls);
    TEST_ASSERT_EQUAL_INT(20260925, f.store.date);
    TEST_ASSERT_FALSE(f.reporter.hasPendingSave());
}

void test_unreadable_state_disables_sending_to_avoid_duplicates() {
    Fixture f;
    f.store.loads = false;
    DailyTemperatureReporter reporter(f.sensors, f.time, f.clock, f.sender, f.store);
    TEST_ASSERT_FALSE(reporter.begin());
    TEST_ASSERT_FALSE(reporter.isReady());
    f.time.current.hour = 15;
    reporter.update(true);
    TEST_ASSERT_EQUAL_INT(0, f.sender.calls);
}

void test_retry_interval_is_wrap_safe() {
    Fixture f;
    f.time.current.hour = 15;
    f.clock.milliseconds = std::numeric_limits<uint32_t>::max() - 1000;
    f.sender.succeeds = false;
    f.reporter.update(true);

    f.clock.milliseconds += 300000;
    f.sender.succeeds = true;
    f.reporter.update(true);
    TEST_ASSERT_EQUAL_INT(2, f.sender.calls);
}

void test_new_local_date_not_elapsed_hours_enables_next_report() {
    Fixture f;
    f.time.current.hour = 15;
    f.reporter.update(true);

    // The local date, not a 24-hour monotonic interval, governs DST days.
    f.time.current.day = 26;
    f.time.current.hour = 15;
    f.clock.milliseconds = 300000;
    f.reporter.update(true);
    TEST_ASSERT_EQUAL_INT(2, f.sender.calls);
}

int main(int, char**) {
    UNITY_BEGIN();
    RUN_TEST(test_sends_at_15_once_with_both_temperatures);
    RUN_TEST(test_sends_after_missed_deadline_when_power_returns);
    RUN_TEST(test_persisted_day_blocks_duplicate_after_restart);
    RUN_TEST(test_waits_for_valid_local_time_and_both_readings);
    RUN_TEST(test_failed_send_retries_after_five_minutes);
    RUN_TEST(test_failed_persistence_never_resends_during_this_boot);
    RUN_TEST(test_unreadable_state_disables_sending_to_avoid_duplicates);
    RUN_TEST(test_retry_interval_is_wrap_safe);
    RUN_TEST(test_new_local_date_not_elapsed_hours_enables_next_report);
    return UNITY_END();
}
