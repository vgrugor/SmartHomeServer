#include "application/SensorValueValidator.h"
#include <cmath>

bool SensorValueValidator::isValid(SensorType type, float value) const {
    if (!std::isfinite(value)) {
        return false;
    }

    switch (type) {
        case SensorType::WATER_LEVEL_LITER:
        case SensorType::BATTERY_VOLTAGE:
            return value >= 0.0f;
        case SensorType::BATTERY_PERCENT:
            return value >= 0.0f && value <= 100.0f;
        case SensorType::HOUSE_TEMP:
        case SensorType::OUTDOOR_TEMP:
        case SensorType::WATER_TEMP:
            return true;
    }

    return false;
}
