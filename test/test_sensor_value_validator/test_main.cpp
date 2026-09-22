#include <limits>
#include <unity.h>
#include "application/SensorValueValidator.h"

void setUp() {}
void tearDown() {}

void test_accepts_finite_temperatures() {
    SensorValueValidator validator;

    TEST_ASSERT_TRUE(validator.isValid(SensorType::HOUSE_TEMP, -25.5f));
    TEST_ASSERT_TRUE(validator.isValid(SensorType::OUTDOOR_TEMP, 36.0f));
    TEST_ASSERT_TRUE(validator.isValid(SensorType::WATER_TEMP, 42.0f));
}

void test_rejects_non_finite_values() {
    SensorValueValidator validator;

    TEST_ASSERT_FALSE(validator.isValid(
        SensorType::HOUSE_TEMP,
        std::numeric_limits<float>::infinity()
    ));
    TEST_ASSERT_FALSE(validator.isValid(
        SensorType::HOUSE_TEMP,
        std::numeric_limits<float>::quiet_NaN()
    ));
}

void test_rejects_negative_volume_and_voltage() {
    SensorValueValidator validator;

    TEST_ASSERT_FALSE(validator.isValid(SensorType::WATER_LEVEL_LITER, -0.1f));
    TEST_ASSERT_FALSE(validator.isValid(SensorType::BATTERY_VOLTAGE, -0.1f));
}

void test_accepts_only_percentage_range() {
    SensorValueValidator validator;

    TEST_ASSERT_TRUE(validator.isValid(SensorType::BATTERY_PERCENT, 0.0f));
    TEST_ASSERT_TRUE(validator.isValid(SensorType::BATTERY_PERCENT, 100.0f));
    TEST_ASSERT_FALSE(validator.isValid(SensorType::BATTERY_PERCENT, -0.1f));
    TEST_ASSERT_FALSE(validator.isValid(SensorType::BATTERY_PERCENT, 100.1f));
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_accepts_finite_temperatures);
    RUN_TEST(test_rejects_non_finite_values);
    RUN_TEST(test_rejects_negative_volume_and_voltage);
    RUN_TEST(test_accepts_only_percentage_range);
    return UNITY_END();
}
