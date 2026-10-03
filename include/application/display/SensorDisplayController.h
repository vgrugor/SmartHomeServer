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
        uint8_t currentPageIndex;
        bool hasCurrentPage;
        bool showingNoData;
        bool hasRenderedPage;
        SensorDisplayPage lastRenderedPage;

        bool isReadingFresh(SensorType type, uint32_t nowMs) const;
        bool isPageAvailable(uint8_t pageIndex, uint32_t nowMs) const;
        bool findNextFreshPage(
            uint8_t startIndex,
            uint32_t nowMs,
            uint8_t& result
        ) const;
        void showNextFreshPage(uint8_t startIndex, uint32_t nowMs);
        DisplayReading readingFor(SensorType type, uint32_t nowMs) const;
        SensorDisplayPage makePage(uint8_t pageIndex, uint32_t nowMs) const;
        void renderCurrentPage(uint32_t nowMs, bool force = false);
};

#endif // SENSOR_DISPLAY_CONTROLLER_H
