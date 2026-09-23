#include "domain/SensorData.h"

void SensorData::setValue(const SensorType& key, float value, uint32_t updatedAtMs)
{
    data[key] = {value, updatedAtMs};
}

float  SensorData::getValue(const SensorType& key) const
{
    auto it = data.find(key);

    if (it != data.end()) {
        return it->second.value;
    }

    return 0.0f;
}

uint32_t SensorData::getUpdatedAtMs(const SensorType& key) const
{
    auto it = data.find(key);

    if (it != data.end()) {
        return it->second.updatedAtMs;
    }

    return 0;
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
