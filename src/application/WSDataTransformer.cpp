#include "application/WSDataTransformer.h"
#include <cstdio>

WsDataTransformer::WsDataTransformer(
    const SensorData& sensorData
) : sensorData(sensorData)
{}

namespace {
    void appendValue(
        std::string& json,
        const char* key,
        float value,
        bool prependComma
    ) {
        char number[48];
        std::snprintf(number, sizeof(number), "%.2f", static_cast<double>(value));

        if (prependComma) {
            json += ',';
        }

        json += '"';
        json += key;
        json += "\":\"";
        json += number;
        json += '"';
    }
}

std::string WsDataTransformer::toJSON() const {
    std::string json;
    json.reserve(160);
    json += '{';

    appendValue(json, "sliderValue1", this->sensorData.getValue(SensorType::HOUSE_TEMP), false);
    appendValue(json, "sliderValue2", this->sensorData.getValue(SensorType::OUTDOOR_TEMP), true);
    appendValue(json, "sliderValue3", this->sensorData.getValue(SensorType::WATER_TEMP), true);
    appendValue(json, "sliderValue4", this->sensorData.getValue(SensorType::WATER_LEVEL_LITER), true);
    appendValue(json, "sliderValue5", this->sensorData.getValue(SensorType::BATTERY_VOLTAGE), true);
    appendValue(json, "sliderValue6", this->sensorData.getValue(SensorType::BATTERY_PERCENT), true);

    json += '}';
    return json;
}
