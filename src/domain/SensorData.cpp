#include "domain/SensorData.h"

void SensorData::setValue(const std::string& key, float value) {
    data[key] = value;
}

float  SensorData::getValue(const std::string& key) const {
    auto it = data.find(key);

    if (it != data.end()) {
        return it->second;
    }

    return 0.0f;
}

bool  SensorData::hasKey(const std::string& key) const {
    return data.find(key) != data.end();
}

void  SensorData::removeKey(const std::string& key) {
    data.erase(key);
}

void  SensorData::clear() {
    data.clear();
}
