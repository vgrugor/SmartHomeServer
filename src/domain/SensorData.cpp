#include "domain/SensorData.h"

void SensorData::setValue(const SensorType& key, float value) {
    data[key] = value;
}

float  SensorData::getValue(const SensorType& key) const {
    auto it = data.find(key);

    if (it != data.end()) {
        return it->second;
    }

    return 0.0f;
}

bool  SensorData::hasKey(const SensorType& key) const {
    return data.find(key) != data.end();
}

void  SensorData::removeKey(const SensorType& key) {
    data.erase(key);
}

void  SensorData::clear() {
    data.clear();
}
