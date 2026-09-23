#include "application/WSDataTransformer.h"
#include <cstdio>

WsDataTransformer::WsDataTransformer(
    const SensorData& sensorData,
    const MonotonicClock& clock
) : sensorData(sensorData),
    clock(clock)
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

    void appendAgeMinutes(
        std::string& json,
        const char* key,
        const SensorData& sensorData,
        SensorType type,
        uint32_t nowMs
    ) {
        json += ",\"";
        json += key;
        json += "\":";

        if (!sensorData.hasKey(type)) {
            json += "null";
            return;
        }

        const uint32_t elapsedMs = nowMs - sensorData.getUpdatedAtMs(type);
        char minutes[16];
        std::snprintf(minutes, sizeof(minutes), "%lu", static_cast<unsigned long>(elapsedMs / 60000UL));
        json += minutes;
    }
}

std::string WsDataTransformer::toJSON() const {
    std::string json;
    json.reserve(320);
    json += '{';

    const uint32_t nowMs = this->clock.now();

    appendValue(json, "sliderValue1", this->sensorData.getValue(SensorType::HOUSE_TEMP), false);
    appendValue(json, "sliderValue2", this->sensorData.getValue(SensorType::OUTDOOR_TEMP), true);
    appendValue(json, "sliderValue3", this->sensorData.getValue(SensorType::WATER_TEMP), true);
    appendValue(json, "sliderValue4", this->sensorData.getValue(SensorType::WATER_LEVEL_LITER), true);
    appendValue(json, "sliderValue5", this->sensorData.getValue(SensorType::BATTERY_VOLTAGE), true);
    appendValue(json, "sliderValue6", this->sensorData.getValue(SensorType::BATTERY_PERCENT), true);
    appendAgeMinutes(json, "sliderValue1AgeMinutes", this->sensorData, SensorType::HOUSE_TEMP, nowMs);
    appendAgeMinutes(json, "sliderValue2AgeMinutes", this->sensorData, SensorType::OUTDOOR_TEMP, nowMs);
    appendAgeMinutes(json, "sliderValue3AgeMinutes", this->sensorData, SensorType::WATER_TEMP, nowMs);
    appendAgeMinutes(json, "sliderValue4AgeMinutes", this->sensorData, SensorType::WATER_LEVEL_LITER, nowMs);
    appendAgeMinutes(json, "sliderValue5AgeMinutes", this->sensorData, SensorType::BATTERY_VOLTAGE, nowMs);
    appendAgeMinutes(json, "sliderValue6AgeMinutes", this->sensorData, SensorType::BATTERY_PERCENT, nowMs);

    json += '}';
    return json;
}
