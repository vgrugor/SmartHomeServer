#ifndef SENSOR_UPDATE_SERVICE_H
#define SENSOR_UPDATE_SERVICE_H

#include "application/SensorUpdateListener.h"
#include "application/SensorValueValidator.h"
#include "application/network/MonotonicClock.h"
#include "domain/SensorData.h"

class SensorUpdateService {
    private:
        SensorData& sensorData;
        const SensorValueValidator& validator;
        SensorUpdateListener& listener;
        const MonotonicClock& clock;
        void saveValue(SensorType type, float value);

    public:
        SensorUpdateService(
            SensorData& sensorData,
            const SensorValueValidator& validator,
            SensorUpdateListener& listener,
            const MonotonicClock& clock
        );
        bool updateValue(SensorType type, float value);
        bool updateShower(
            float temperature,
            float liters,
            float voltage,
            float percent
        );
};

#endif // SENSOR_UPDATE_SERVICE_H
