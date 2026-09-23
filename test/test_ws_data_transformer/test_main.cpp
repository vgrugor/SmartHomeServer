#include <unity.h>
#include "application/WSDataTransformer.h"

void setUp() {}
void tearDown() {}

void test_serializes_empty_state_with_all_six_keys() {
    SensorData data;
    WsDataTransformer transformer(data);

    const std::string json = transformer.toJSON();

    TEST_ASSERT_EQUAL_STRING(
        "{\"sliderValue1\":\"0.00\",\"sliderValue2\":\"0.00\","
        "\"sliderValue3\":\"0.00\",\"sliderValue4\":\"0.00\","
        "\"sliderValue5\":\"0.00\",\"sliderValue6\":\"0.00\"}",
        json.c_str()
    );
}

void test_maps_each_sensor_to_the_correct_json_key() {
    SensorData data;
    data.setValue(SensorType::HOUSE_TEMP, 1.25f);
    data.setValue(SensorType::OUTDOOR_TEMP, 2.50f);
    data.setValue(SensorType::WATER_TEMP, 3.75f);
    data.setValue(SensorType::WATER_LEVEL_LITER, 4.00f);
    data.setValue(SensorType::BATTERY_VOLTAGE, 5.50f);
    data.setValue(SensorType::BATTERY_PERCENT, 6.25f);
    WsDataTransformer transformer(data);

    const std::string json = transformer.toJSON();

    TEST_ASSERT_EQUAL_STRING(
        "{\"sliderValue1\":\"1.25\",\"sliderValue2\":\"2.50\","
        "\"sliderValue3\":\"3.75\",\"sliderValue4\":\"4.00\","
        "\"sliderValue5\":\"5.50\",\"sliderValue6\":\"6.25\"}",
        json.c_str()
    );
}

void test_preserves_negative_and_fractional_values_to_two_decimals() {
    SensorData data;
    data.setValue(SensorType::HOUSE_TEMP, -12.345f);
    data.setValue(SensorType::OUTDOOR_TEMP, 7.5f);
    WsDataTransformer transformer(data);

    const std::string json = transformer.toJSON();

    TEST_ASSERT_EQUAL_STRING(
        "{\"sliderValue1\":\"-12.35\",\"sliderValue2\":\"7.50\","
        "\"sliderValue3\":\"0.00\",\"sliderValue4\":\"0.00\","
        "\"sliderValue5\":\"0.00\",\"sliderValue6\":\"0.00\"}",
        json.c_str()
    );
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_serializes_empty_state_with_all_six_keys);
    RUN_TEST(test_maps_each_sensor_to_the_correct_json_key);
    RUN_TEST(test_preserves_negative_and_fractional_values_to_two_decimals);
    return UNITY_END();
}
