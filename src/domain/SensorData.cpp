#include "domain/SensorData.h"

void SensorData::setValue(const SensorType& key, float value)
{
    data[key] = value;

    this->pushEvent(key, value);
}

float  SensorData::getValue(const SensorType& key) const
{
    auto it = data.find(key);

    if (it != data.end()) {
        return it->second;
    }

    return 0.0f;
}

bool  SensorData::hasKey(const SensorType& key) const
{
    return data.find(key) != data.end();
}

void  SensorData::removeKey(const SensorType& key)
{
    data.erase(key);
}

void  SensorData::clear() {
    data.clear();
}

void  SensorData::pushEvent(const SensorType& key, float value)
{
    String valueString = String(value, 2);
    String message = "";

    if (SensorType::HOUSE_TEMP == key) {
        message = "Set house temperature: " + valueString + "°C";
    } else if (SensorType::OUTDOOR_TEMP == key) {
        message = "Set outdoor temperature: " + valueString + "°C";
    } else if (SensorType::WATER_TEMP == key) {
        message = "Set shower water temperature: " + valueString + "°C";
    } else if (SensorType::WATER_LEVEL_LITER == key) {
        message = "Set shower water level liters: " + valueString + "l";
    } else if (SensorType::BATTERY_VOLTAGE == key) {
        message = "Set shower battery voltage: " + valueString + "v";
    } else if (SensorType::BATTERY_PERCENT == key) {
        message = "Set battery level percent: " + valueString + "%";
    }

    EventNotifier::getInstance().notifyObservers(EventType::PARAM_SAVED, valueString);
}
