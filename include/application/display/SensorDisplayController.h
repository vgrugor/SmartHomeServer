#ifndef SENSOR_DISPLAY_CONTROLLER_H
#define SENSOR_DISPLAY_CONTROLLER_H

#include <cstdint>
#include "application/display/SensorDisplayView.h"
#include "application/network/MonotonicClock.h"
#include "domain/SensorData.h"

class SensorDisplayController {
    public:
        SensorDisplayController(
            const SensorData& sensorData,
            const MonotonicClock& clock,
            SensorDisplayView& view,
            uint32_t pageDurationMs
        );

        void begin();
        void update();

    private:
        const SensorData& sensorData;
        const MonotonicClock& clock;
        SensorDisplayView& view;
        uint32_t pageDurationMs;
        uint32_t lastPageChangeMs;
        uint32_t lastRenderedUpdateMs;
        uint8_t currentPageIndex;
        bool hasCurrentPage;

        bool isFresh(uint8_t pageIndex, uint32_t nowMs) const;
        bool findNextFreshPage(
            uint8_t startIndex,
            uint32_t nowMs,
            uint8_t& result
        ) const;
        void showNextFreshPage(uint8_t startIndex, uint32_t nowMs);
        void renderCurrentPage(uint32_t nowMs);
};

#endif // SENSOR_DISPLAY_CONTROLLER_H
