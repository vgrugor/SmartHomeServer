#include <cstring>
#include <unity.h>
#include "application/display/SensorDisplayController.h"

class FakeClock : public MonotonicClock {
    public:
        uint32_t currentTime = 0;

        uint32_t now() const override {
            return currentTime;
        }
};

class RecordingDisplayView : public SensorDisplayView {
    public:
        int beginCount = 0;
        int showCount = 0;
        int noFreshDataCount = 0;
        SensorDisplayPage lastPage = {};

        void begin() override {
            beginCount++;
        }

        void show(const SensorDisplayPage& page) override {
            lastPage = page;
            showCount++;
        }

        void showNoFreshData() override {
            noFreshDataCount++;
        }
};

void setUp() {}
void tearDown() {}

void test_begins_view_and_immediately_shows_house_page() {
    SensorData data;
    data.setValue(SensorType::HOUSE_TEMP, 21.75f, 1000);
    FakeClock clock;
    clock.currentTime = 61000;
    RecordingDisplayView view;
    SensorDisplayController controller(data, clock, view, 5000);

    controller.begin();

    TEST_ASSERT_EQUAL_INT(1, view.beginCount);
    TEST_ASSERT_EQUAL_INT(1, view.showCount);
    TEST_ASSERT_EQUAL_STRING("HOUSE", view.lastPage.label);
    TEST_ASSERT_EQUAL_STRING("C", view.lastPage.unit);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 21.75f, view.lastPage.value);
    TEST_ASSERT_EQUAL_UINT32(1, view.lastPage.ageMinutes);
    TEST_ASSERT_EQUAL_UINT8(1, view.lastPage.decimalPlaces);
    TEST_ASSERT_EQUAL_INT(0, view.noFreshDataCount);
}

void test_cycles_all_six_large_value_pages_every_five_seconds() {
    SensorData data;
    data.setValue(SensorType::HOUSE_TEMP, 1.0f, 0);
    data.setValue(SensorType::OUTDOOR_TEMP, 2.0f, 0);
    data.setValue(SensorType::WATER_TEMP, 3.0f, 0);
    data.setValue(SensorType::WATER_LEVEL_LITER, 4.0f, 0);
    data.setValue(SensorType::BATTERY_VOLTAGE, 5.0f, 0);
    data.setValue(SensorType::BATTERY_PERCENT, 6.0f, 0);
    FakeClock clock;
    RecordingDisplayView view;
    SensorDisplayController controller(data, clock, view, 5000);
    controller.begin();

    const char* expectedLabels[] = {
        "OUTSIDE",
        "SHOWER",
        "WATER",
        "BATTERY VOLT",
        "BATTERY LEVEL",
        "HOUSE",
    };

    for (int index = 0; index < 6; index++) {
        clock.currentTime += 4999;
        controller.update();
        TEST_ASSERT_EQUAL_INT(index + 1, view.showCount);

        clock.currentTime += 1;
        controller.update();
        TEST_ASSERT_EQUAL_INT(index + 2, view.showCount);
        TEST_ASSERT_EQUAL_STRING(expectedLabels[index], view.lastPage.label);
    }
}

void test_skips_missing_and_hour_old_values() {
    SensorData data;
    FakeClock clock;
    clock.currentTime = 3600000;
    data.setValue(SensorType::HOUSE_TEMP, 21.0f, 0);
    data.setValue(SensorType::OUTDOOR_TEMP, 8.5f, clock.currentTime);
    RecordingDisplayView view;
    SensorDisplayController controller(data, clock, view, 5000);

    controller.begin();
    TEST_ASSERT_EQUAL_INT(1, view.showCount);
    TEST_ASSERT_EQUAL_STRING("OUTSIDE", view.lastPage.label);
    TEST_ASSERT_EQUAL_INT(0, view.noFreshDataCount);

    clock.currentTime += 3600000;
    controller.update();

    TEST_ASSERT_EQUAL_INT(1, view.showCount);
    TEST_ASSERT_EQUAL_INT(1, view.noFreshDataCount);
}

void test_refreshes_current_page_immediately_when_its_value_changes() {
    SensorData data;
    FakeClock clock;
    RecordingDisplayView view;
    SensorDisplayController controller(data, clock, view, 5000);
    controller.begin();
    TEST_ASSERT_EQUAL_INT(1, view.noFreshDataCount);
    TEST_ASSERT_EQUAL_INT(0, view.showCount);

    clock.currentTime = 1000;
    data.setValue(SensorType::HOUSE_TEMP, 23.5f, clock.currentTime);
    controller.update();

    TEST_ASSERT_EQUAL_INT(1, view.showCount);
    TEST_ASSERT_EQUAL_STRING("HOUSE", view.lastPage.label);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 23.5f, view.lastPage.value);
}

void test_page_timer_handles_millis_overflow() {
    SensorData data;
    FakeClock clock;
    clock.currentTime = UINT32_MAX - 2000;
    data.setValue(SensorType::HOUSE_TEMP, 21.0f, clock.currentTime);
    data.setValue(SensorType::OUTDOOR_TEMP, 8.0f, clock.currentTime);
    RecordingDisplayView view;
    SensorDisplayController controller(data, clock, view, 5000);
    controller.begin();

    clock.currentTime = 2998;
    controller.update();
    TEST_ASSERT_EQUAL_INT(1, view.showCount);

    clock.currentTime = 2999;
    controller.update();
    TEST_ASSERT_EQUAL_INT(2, view.showCount);
    TEST_ASSERT_EQUAL_STRING("OUTSIDE", view.lastPage.label);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_begins_view_and_immediately_shows_house_page);
    RUN_TEST(test_cycles_all_six_large_value_pages_every_five_seconds);
    RUN_TEST(test_skips_missing_and_hour_old_values);
    RUN_TEST(test_refreshes_current_page_immediately_when_its_value_changes);
    RUN_TEST(test_page_timer_handles_millis_overflow);
    return UNITY_END();
}
