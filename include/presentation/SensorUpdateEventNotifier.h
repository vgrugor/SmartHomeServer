#ifndef SENSOR_UPDATE_EVENT_NOTIFIER_H
#define SENSOR_UPDATE_EVENT_NOTIFIER_H

#include "application/SensorUpdateListener.h"

class SensorUpdateEventNotifier : public SensorUpdateListener {
    public:
        void onSensorValueSaved(SensorType type, float value) override;
        void onSensorValuesUpdated() override;
};

#endif // SENSOR_UPDATE_EVENT_NOTIFIER_H
