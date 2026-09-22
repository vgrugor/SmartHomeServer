#ifndef SENSOR_UPDATE_LISTENER_H
#define SENSOR_UPDATE_LISTENER_H

#include "domain/SensorType.h"

class SensorUpdateListener {
    public:
        virtual ~SensorUpdateListener() = default;
        virtual void onSensorValueSaved(SensorType type, float value) = 0;
        virtual void onSensorValuesUpdated() = 0;
};

#endif // SENSOR_UPDATE_LISTENER_H
