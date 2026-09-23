#ifndef SENSOR_DISPLAY_VIEW_H
#define SENSOR_DISPLAY_VIEW_H

#include <cstdint>

struct SensorDisplayPage {
    const char* label;
    const char* unit;
    float value;
    uint32_t ageMinutes;
    uint8_t decimalPlaces;
};

class SensorDisplayView {
    public:
        virtual ~SensorDisplayView() = default;
        virtual void begin() = 0;
        virtual void show(const SensorDisplayPage& page) = 0;
        virtual void showNoFreshData() = 0;
};

#endif // SENSOR_DISPLAY_VIEW_H
