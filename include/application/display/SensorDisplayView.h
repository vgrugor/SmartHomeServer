#ifndef SENSOR_DISPLAY_VIEW_H
#define SENSOR_DISPLAY_VIEW_H

#include <cstdint>

enum class SensorDisplayPageKind : uint8_t { TEMPERATURES, SHOWER };

struct DisplayReading {
    bool available;
    float value;
    uint32_t ageMinutes;
};

struct SensorDisplayPage {
    SensorDisplayPageKind kind;
    DisplayReading houseTemperature;
    DisplayReading outdoorTemperature;
    DisplayReading showerTemperature;
    DisplayReading showerVolume;
    DisplayReading batteryVoltage;
    DisplayReading batteryPercent;
};

class SensorDisplayView {
    public:
        virtual ~SensorDisplayView() = default;
        virtual void begin() = 0;
        virtual void show(const SensorDisplayPage& page) = 0;
        virtual void showNoFreshData() = 0;
};

#endif // SENSOR_DISPLAY_VIEW_H
