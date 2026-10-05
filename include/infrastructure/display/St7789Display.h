#ifndef ST7789_DISPLAY_H
#define ST7789_DISPLAY_H

#include <cstddef>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "application/display/SensorDisplayView.h"

class St7789Display : public SensorDisplayView {
    public:
        St7789Display(int chipSelectPin, int dataCommandPin, int resetPin);
        void begin() override;
        void show(const SensorDisplayPage& page) override;
        void showNoFreshData() override;

    private:
        Adafruit_ST7789 display;

        void drawCenteredText(
            const char* text,
            int16_t y,
            uint8_t size,
            uint16_t color
        );
        void drawTextInRegion(
            const char* text,
            int16_t left,
            int16_t width,
            int16_t y,
            uint8_t maxSize,
            uint16_t color
        );
        void drawHouseIcon(int16_t x, int16_t y, uint16_t color);
        void drawSunIcon(int16_t x, int16_t y, uint16_t color);
        void drawTemperatureSection(
            const DisplayReading& reading,
            int16_t top,
            bool inside
        );
        void drawShowerRow(
            const char* label,
            const DisplayReading& reading,
            const char* unit,
            uint8_t decimals,
            int16_t top
        );
        void formatAge(const DisplayReading& reading, char* output, size_t size) const;
};

#endif // ST7789_DISPLAY_H
