#include <limits>
#include <unity.h>
#include "application/SensorUpdateService.h"

struct SavedValueEvent {
    SensorType type;
    float value;
};

class RecordingSensorUpdateListener : public SensorUpdateListener {
    public:
        SavedValueEvent savedEvents[6];
        int savedEventCount = 0;
        int valuesUpdatedEventCount = 0;

        void onSensorValueSaved(SensorType type, float value) override {
            savedEvents[savedEventCount] = {type, value};
            savedEventCount++;
        }

        void onSensorValuesUpdated() override {
            valuesUpdatedEventCount++;
        }
};

void setUp() {}
void tearDown() {}

void test_updates_each_sensor_type() {
    SensorData data;
    SensorValueValidator validator;
    RecordingSensorUpdateListener listener;
    SensorUpdateService service(data, validator, listener);

    TEST_ASSERT_TRUE(service.updateValue(SensorType::HOUSE_TEMP, 21.5f));
    TEST_ASSERT_TRUE(service.updateValue(SensorType::OUTDOOR_TEMP, -4.0f));
    TEST_ASSERT_TRUE(service.updateValue(SensorType::WATER_TEMP, 40.0f));
    TEST_ASSERT_TRUE(service.updateValue(SensorType::WATER_LEVEL_LITER, 35.0f));
    TEST_ASSERT_TRUE(service.updateValue(SensorType::BATTERY_VOLTAGE, 12.4f));
    TEST_ASSERT_TRUE(service.updateValue(SensorType::BATTERY_PERCENT, 88.0f));

    TEST_ASSERT_FLOAT_WITHIN(0.001f, 21.5f, data.getValue(SensorType::HOUSE_TEMP));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -4.0f, data.getValue(SensorType::OUTDOOR_TEMP));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 40.0f, data.getValue(SensorType::WATER_TEMP));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 35.0f, data.getValue(SensorType::WATER_LEVEL_LITER));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 12.4f, data.getValue(SensorType::BATTERY_VOLTAGE));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 88.0f, data.getValue(SensorType::BATTERY_PERCENT));
    TEST_ASSERT_EQUAL_INT(6, listener.savedEventCount);
    TEST_ASSERT_EQUAL_INT(6, listener.valuesUpdatedEventCount);
}

void test_rejects_invalid_values_without_events() {
    SensorData data;
    SensorValueValidator validator;
    RecordingSensorUpdateListener listener;
    SensorUpdateService service(data, validator, listener);

    TEST_ASSERT_FALSE(service.updateValue(
        SensorType::HOUSE_TEMP,
        std::numeric_limits<float>::quiet_NaN()
    ));
    TEST_ASSERT_FALSE(service.updateValue(
        SensorType::OUTDOOR_TEMP,
        std::numeric_limits<float>::infinity()
    ));
    TEST_ASSERT_FALSE(service.updateValue(SensorType::WATER_LEVEL_LITER, -0.1f));
    TEST_ASSERT_FALSE(service.updateValue(SensorType::BATTERY_VOLTAGE, -0.1f));
    TEST_ASSERT_FALSE(service.updateValue(SensorType::BATTERY_PERCENT, -0.1f));
    TEST_ASSERT_FALSE(service.updateValue(SensorType::BATTERY_PERCENT, 100.1f));

    TEST_ASSERT_FALSE(data.hasKey(SensorType::HOUSE_TEMP));
    TEST_ASSERT_FALSE(data.hasKey(SensorType::OUTDOOR_TEMP));
    TEST_ASSERT_FALSE(data.hasKey(SensorType::WATER_LEVEL_LITER));
    TEST_ASSERT_FALSE(data.hasKey(SensorType::BATTERY_VOLTAGE));
    TEST_ASSERT_FALSE(data.hasKey(SensorType::BATTERY_PERCENT));
    TEST_ASSERT_EQUAL_INT(0, listener.savedEventCount);
    TEST_ASSERT_EQUAL_INT(0, listener.valuesUpdatedEventCount);
}

void test_invalid_shower_update_is_atomic() {
    SensorData data;
    data.setValue(SensorType::WATER_TEMP, 20.0f);
    data.setValue(SensorType::WATER_LEVEL_LITER, 10.0f);
    data.setValue(SensorType::BATTERY_VOLTAGE, 11.0f);
    data.setValue(SensorType::BATTERY_PERCENT, 50.0f);

    SensorValueValidator validator;
    RecordingSensorUpdateListener listener;
    SensorUpdateService service(data, validator, listener);

    TEST_ASSERT_FALSE(service.updateShower(42.0f, 30.0f, 12.5f, 101.0f));

    TEST_ASSERT_FLOAT_WITHIN(0.001f, 20.0f, data.getValue(SensorType::WATER_TEMP));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 10.0f, data.getValue(SensorType::WATER_LEVEL_LITER));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 11.0f, data.getValue(SensorType::BATTERY_VOLTAGE));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 50.0f, data.getValue(SensorType::BATTERY_PERCENT));
    TEST_ASSERT_EQUAL_INT(0, listener.savedEventCount);
    TEST_ASSERT_EQUAL_INT(0, listener.valuesUpdatedEventCount);
}

void test_valid_shower_update_emits_four_saved_events_and_one_update_event() {
    SensorData data;
    SensorValueValidator validator;
    RecordingSensorUpdateListener listener;
    SensorUpdateService service(data, validator, listener);

    TEST_ASSERT_TRUE(service.updateShower(42.0f, 30.0f, 12.5f, 75.0f));

    TEST_ASSERT_FLOAT_WITHIN(0.001f, 42.0f, data.getValue(SensorType::WATER_TEMP));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 30.0f, data.getValue(SensorType::WATER_LEVEL_LITER));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 12.5f, data.getValue(SensorType::BATTERY_VOLTAGE));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 75.0f, data.getValue(SensorType::BATTERY_PERCENT));
    TEST_ASSERT_EQUAL_INT(4, listener.savedEventCount);
    TEST_ASSERT_EQUAL_INT(1, listener.valuesUpdatedEventCount);
    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(SensorType::WATER_TEMP),
        static_cast<int>(listener.savedEvents[0].type)
    );
    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(SensorType::WATER_LEVEL_LITER),
        static_cast<int>(listener.savedEvents[1].type)
    );
    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(SensorType::BATTERY_VOLTAGE),
        static_cast<int>(listener.savedEvents[2].type)
    );
    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(SensorType::BATTERY_PERCENT),
        static_cast<int>(listener.savedEvents[3].type)
    );
}

void test_single_update_emits_saved_value_and_one_update_event() {
    SensorData data;
    SensorValueValidator validator;
    RecordingSensorUpdateListener listener;
    SensorUpdateService service(data, validator, listener);

    TEST_ASSERT_TRUE(service.updateValue(SensorType::HOUSE_TEMP, 23.75f));

    TEST_ASSERT_EQUAL_INT(1, listener.savedEventCount);
    TEST_ASSERT_EQUAL_INT(1, listener.valuesUpdatedEventCount);
    TEST_ASSERT_EQUAL_INT(
        static_cast<int>(SensorType::HOUSE_TEMP),
        static_cast<int>(listener.savedEvents[0].type)
    );
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 23.75f, listener.savedEvents[0].value);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_updates_each_sensor_type);
    RUN_TEST(test_rejects_invalid_values_without_events);
    RUN_TEST(test_invalid_shower_update_is_atomic);
    RUN_TEST(test_valid_shower_update_emits_four_saved_events_and_one_update_event);
    RUN_TEST(test_single_update_emits_saved_value_and_one_update_event);
    return UNITY_END();
}
