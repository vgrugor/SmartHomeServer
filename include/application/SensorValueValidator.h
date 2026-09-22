#ifndef SENSOR_VALUE_VALIDATOR_H
#define SENSOR_VALUE_VALIDATOR_H

#include "domain/SensorType.h"

class SensorValueValidator {
    public:
        bool isValid(SensorType type, float value) const;
};

#endif // SENSOR_VALUE_VALIDATOR_H
