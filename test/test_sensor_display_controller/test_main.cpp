#include <unity.h>
#include "application/display/SensorDisplayController.h"

class FakeClock : public MonotonicClock {
    public:
        uint32_t currentTime = 0;

        uint32_t now() const override { return currentTime; }
};

class RecordingDisplayView : public SensorDisplayView {
    public:
        int beginCount = 0;
        int showCount = 0;
        int noFreshDataCount = 0;
        SensorDisplayPage lastPage = {};

        void begin() override { beginCount++; }
        void show(const SensorDisplayPage& page) override {
            lastPage = page;
            showCount++;
        }
        void showNoFreshData() override { noFreshDataCount++; }
};

void setUp() {}
void tearDown() {}

void setShower(SensorData& data, uint32_t at) {
    data.setValue(SensorType::WATER_TEMP, 39.5f, at);
    data.setValue(SensorType::WATER_LEVEL_LITER, 75.0f, at);
    data.setValue(SensorType::BATTERY_VOLTAGE, 3.8f, at);
    data.setValue(SensorType::BATTERY_PERCENT, 80.0f, at);
}

void test_temperature_page_contains_both_readings() {
    SensorData data;
    data.setValue(SensorType::HOUSE_TEMP, 21.75f, 1000);
    data.setValue(SensorType::OUTDOOR_TEMP, 8.5f, 31000);
    FakeClock clock;
    clock.currentTime = 61000;
    RecordingDisplayView view;
    SensorDisplayController controller(data, clock, view, 5000);

    controller.begin();

    TEST_ASSERT_EQUAL_INT(1, view.beginCount);
    TEST_ASSERT_EQUAL_INT(1, view.showCount);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(SensorDisplayPageKind::TEMPERATURES),
        static_cast<int>(view.lastPage.kind));
    TEST_ASSERT_TRUE(view.lastPage.houseTemperature.available);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 21.75f, view.lastPage.houseTemperature.value);
    TEST_ASSERT_EQUAL_UINT32(1, view.lastPage.houseTemperature.ageMinutes);
    TEST_ASSERT_TRUE(view.lastPage.outdoorTemperature.available);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 8.5f, view.lastPage.outdoorTemperature.value);
    TEST_ASSERT_EQUAL_UINT32(0, view.lastPage.outdoorTemperature.ageMinutes);
}

void test_rotates_only_two_pages_every_five_seconds() {
    SensorData data;
    data.setValue(SensorType::HOUSE_TEMP, 21.0f, 0);
    data.setValue(SensorType::OUTDOOR_TEMP, 8.0f, 0);
    setShower(data, 0);
    FakeClock clock;
    RecordingDisplayView view;
    SensorDisplayController controller(data, clock, view, 5000);
    controller.begin();

    clock.currentTime = 4999;
    controller.update();
    TEST_ASSERT_EQUAL_INT(1, view.showCount);
    clock.currentTime = 5000;
    controller.update();
    TEST_ASSERT_EQUAL_INT(2, view.showCount);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(SensorDisplayPageKind::SHOWER),
        static_cast<int>(view.lastPage.kind));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 39.5f, view.lastPage.showerTemperature.value);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 75.0f, view.lastPage.showerVolume.value);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 3.8f, view.lastPage.batteryVoltage.value);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 80.0f, view.lastPage.batteryPercent.value);
    clock.currentTime = 10000;
    controller.update();
    TEST_ASSERT_EQUAL_INT(3, view.showCount);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(SensorDisplayPageKind::TEMPERATURES),
        static_cast<int>(view.lastPage.kind));
}

void test_stale_or_missing_temperature_is_not_included() {
    SensorData data;
    FakeClock clock;
    clock.currentTime = 3600000;
    data.setValue(SensorType::HOUSE_TEMP, 21.0f, 0);
    data.setValue(SensorType::OUTDOOR_TEMP, 8.5f, clock.currentTime);
    RecordingDisplayView view;
    SensorDisplayController controller(data, clock, view, 5000);

    controller.begin();
    TEST_ASSERT_FALSE(view.lastPage.houseTemperature.available);
    TEST_ASSERT_TRUE(view.lastPage.outdoorTemperature.available);
    TEST_ASSERT_EQUAL_INT(0, view.noFreshDataCount);

    clock.currentTime += 3600000;
    controller.update();
    TEST_ASSERT_EQUAL_INT(1, view.noFreshDataCount);
    controller.update();
    TEST_ASSERT_EQUAL_INT(1, view.noFreshDataCount);
}

void test_stale_shower_leaves_temperature_page_without_redrawing_every_cycle() {
    SensorData data;
    FakeClock clock;
    clock.currentTime = 3600000;
    data.setValue(SensorType::HOUSE_TEMP, 21.0f, clock.currentTime);
    setShower(data, 0);
    RecordingDisplayView view;
    SensorDisplayController controller(data, clock, view, 5000);
    controller.begin();

    clock.currentTime += 5000;
    controller.update();
    TEST_ASSERT_EQUAL_INT(1, view.showCount);
    TEST_ASSERT_EQUAL_INT(0, view.noFreshDataCount);
}

void test_shower_only_is_shown_when_temperatures_are_stale() {
    SensorData data;
    FakeClock clock;
    clock.currentTime = 3600000;
    data.setValue(SensorType::HOUSE_TEMP, 21.0f, 0);
    setShower(data, clock.currentTime);
    RecordingDisplayView view;
    SensorDisplayController controller(data, clock, view, 5000);
    controller.begin();

    TEST_ASSERT_EQUAL_INT(static_cast<int>(SensorDisplayPageKind::SHOWER),
        static_cast<int>(view.lastPage.kind));
    TEST_ASSERT_EQUAL_INT(0, view.noFreshDataCount);
}

void test_partial_shower_batch_is_not_shown() {
    SensorData data;
    data.setValue(SensorType::WATER_TEMP, 39.5f, 0);
    data.setValue(SensorType::WATER_LEVEL_LITER, 75.0f, 0);
    data.setValue(SensorType::BATTERY_VOLTAGE, 3.8f, 0);
    FakeClock clock;
    RecordingDisplayView view;
    SensorDisplayController controller(data, clock, view, 5000);
    controller.begin();

    TEST_ASSERT_EQUAL_INT(0, view.showCount);
    TEST_ASSERT_EQUAL_INT(1, view.noFreshDataCount);
}

void test_expired_shower_page_returns_to_fresh_temperatures() {
    SensorData data;
    FakeClock clock;
    data.setValue(SensorType::HOUSE_TEMP, 21.0f, 0);
    setShower(data, 0);
    RecordingDisplayView view;
    SensorDisplayController controller(data, clock, view, 5000);
    controller.begin();

    clock.currentTime = 5000;
    controller.update();
    TEST_ASSERT_EQUAL_INT(static_cast<int>(SensorDisplayPageKind::SHOWER),
        static_cast<int>(view.lastPage.kind));

    clock.currentTime = 3600000;
    data.setValue(SensorType::HOUSE_TEMP, 22.0f, clock.currentTime);
    controller.update();
    TEST_ASSERT_EQUAL_INT(static_cast<int>(SensorDisplayPageKind::TEMPERATURES),
        static_cast<int>(view.lastPage.kind));
    TEST_ASSERT_EQUAL_INT(0, view.noFreshDataCount);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 22.0f, view.lastPage.houseTemperature.value);
}

void test_refreshes_temperature_page_when_one_value_changes_or_expires() {
    SensorData data;
    FakeClock clock;
    RecordingDisplayView view;
    SensorDisplayController controller(data, clock, view, 5000);
    controller.begin();
    TEST_ASSERT_EQUAL_INT(1, view.noFreshDataCount);

    clock.currentTime = 1000;
    data.setValue(SensorType::HOUSE_TEMP, 23.5f, 1000);
    controller.update();
    TEST_ASSERT_EQUAL_INT(1, view.showCount);
    TEST_ASSERT_FALSE(view.lastPage.outdoorTemperature.available);

    clock.currentTime = 2000;
    data.setValue(SensorType::OUTDOOR_TEMP, 8.0f, 2000);
    controller.update();
    TEST_ASSERT_EQUAL_INT(2, view.showCount);
    TEST_ASSERT_TRUE(view.lastPage.outdoorTemperature.available);

    clock.currentTime = 3601000;
    controller.update();
    TEST_ASSERT_EQUAL_INT(3, view.showCount);
    TEST_ASSERT_FALSE(view.lastPage.houseTemperature.available);
    TEST_ASSERT_TRUE(view.lastPage.outdoorTemperature.available);
}

void test_page_timer_handles_millis_overflow() {
    SensorData data;
    FakeClock clock;
    clock.currentTime = UINT32_MAX - 2000;
    data.setValue(SensorType::HOUSE_TEMP, 21.0f, clock.currentTime);
    setShower(data, clock.currentTime);
    RecordingDisplayView view;
    SensorDisplayController controller(data, clock, view, 5000);
    controller.begin();

    clock.currentTime = 2998;
    controller.update();
    TEST_ASSERT_EQUAL_INT(1, view.showCount);
    clock.currentTime = 2999;
    controller.update();
    TEST_ASSERT_EQUAL_INT(2, view.showCount);
    TEST_ASSERT_EQUAL_INT(static_cast<int>(SensorDisplayPageKind::SHOWER),
        static_cast<int>(view.lastPage.kind));
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_temperature_page_contains_both_readings);
    RUN_TEST(test_rotates_only_two_pages_every_five_seconds);
    RUN_TEST(test_stale_or_missing_temperature_is_not_included);
    RUN_TEST(test_stale_shower_leaves_temperature_page_without_redrawing_every_cycle);
    RUN_TEST(test_shower_only_is_shown_when_temperatures_are_stale);
    RUN_TEST(test_partial_shower_batch_is_not_shown);
    RUN_TEST(test_expired_shower_page_returns_to_fresh_temperatures);
    RUN_TEST(test_refreshes_temperature_page_when_one_value_changes_or_expires);
    RUN_TEST(test_page_timer_handles_millis_overflow);
    return UNITY_END();
}
