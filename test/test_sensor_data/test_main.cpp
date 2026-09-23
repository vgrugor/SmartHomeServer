#include <unity.h>
#include "domain/SensorData.h"

void setUp() {}
void tearDown() {}

void test_returns_zero_for_missing_value() {
    SensorData sensorData;

    TEST_ASSERT_FALSE(sensorData.hasKey(SensorType::HOUSE_TEMP));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, sensorData.getValue(SensorType::HOUSE_TEMP));
}

void test_stores_and_replaces_value() {
    SensorData sensorData;

    sensorData.setValue(SensorType::OUTDOOR_TEMP, 12.5f, 1000);
    TEST_ASSERT_TRUE(sensorData.hasKey(SensorType::OUTDOOR_TEMP));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 12.5f, sensorData.getValue(SensorType::OUTDOOR_TEMP));
    TEST_ASSERT_EQUAL_UINT32(1000, sensorData.getUpdatedAtMs(SensorType::OUTDOOR_TEMP));

    sensorData.setValue(SensorType::OUTDOOR_TEMP, -3.25f, 2500);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -3.25f, sensorData.getValue(SensorType::OUTDOOR_TEMP));
    TEST_ASSERT_EQUAL_UINT32(2500, sensorData.getUpdatedAtMs(SensorType::OUTDOOR_TEMP));
}

void test_removes_one_value() {
    SensorData sensorData;
    sensorData.setValue(SensorType::HOUSE_TEMP, 21.0f);
    sensorData.setValue(SensorType::OUTDOOR_TEMP, 7.0f);

    sensorData.removeKey(SensorType::HOUSE_TEMP);

    TEST_ASSERT_FALSE(sensorData.hasKey(SensorType::HOUSE_TEMP));
    TEST_ASSERT_TRUE(sensorData.hasKey(SensorType::OUTDOOR_TEMP));
}

void test_clears_all_values() {
    SensorData sensorData;
    sensorData.setValue(SensorType::BATTERY_VOLTAGE, 12.4f);
    sensorData.setValue(SensorType::BATTERY_PERCENT, 88.0f);

    sensorData.clear();

    TEST_ASSERT_FALSE(sensorData.hasKey(SensorType::BATTERY_VOLTAGE));
    TEST_ASSERT_FALSE(sensorData.hasKey(SensorType::BATTERY_PERCENT));
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_returns_zero_for_missing_value);
    RUN_TEST(test_stores_and_replaces_value);
    RUN_TEST(test_removes_one_value);
    RUN_TEST(test_clears_all_values);
    return UNITY_END();
}
