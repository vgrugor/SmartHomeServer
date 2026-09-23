#include "infrastructure/display/St7789Display.h"
#include <cstdio>
#include <cstring>

namespace {
    const uint16_t BACKGROUND_COLOR = ST77XX_BLACK;
    const uint16_t HEADER_COLOR = ST77XX_CYAN;
    const uint16_t FRESH_VALUE_COLOR = ST77XX_WHITE;
    const uint16_t NO_DATA_COLOR = 0x7BEF;
    const uint16_t AGE_COLOR = 0xBDF7;
}

St7789Display::St7789Display(
    int chipSelectPin,
    int dataCommandPin,
    int resetPin
) : display(chipSelectPin, dataCommandPin, resetPin)
{}

void St7789Display::begin() {
    this->display.init(240, 320);
    this->display.setRotation(1);
    this->display.setTextWrap(false);
    this->display.fillScreen(BACKGROUND_COLOR);
}

void St7789Display::show(const SensorDisplayPage& page) {
    this->display.fillScreen(BACKGROUND_COLOR);
    this->drawCenteredText(page.label, 18, 3, HEADER_COLOR);

    char value[32];
    std::snprintf(
        value,
        sizeof(value),
        "%.*f %s",
        page.decimalPlaces,
        static_cast<double>(page.value),
        page.unit
    );

    uint8_t valueSize = 8;
    while (valueSize > 3 && std::strlen(value) * 6U * valueSize > 304U) {
        valueSize--;
    }

    this->drawCenteredText(
        value,
        88,
        valueSize,
        FRESH_VALUE_COLOR
    );

    char age[32];
    this->formatAge(page, age, sizeof(age));
    this->drawCenteredText(age, 211, 2, AGE_COLOR);
}

void St7789Display::showNoFreshData() {
    this->display.fillScreen(BACKGROUND_COLOR);
    this->drawCenteredText("NO FRESH", 82, 5, NO_DATA_COLOR);
    this->drawCenteredText("DATA", 145, 5, NO_DATA_COLOR);
}

void St7789Display::drawCenteredText(
    const char* text,
    int16_t y,
    uint8_t size,
    uint16_t color
) {
    int16_t boundsX;
    int16_t boundsY;
    uint16_t width;
    uint16_t height;

    this->display.setTextSize(size);
    this->display.setTextColor(color, BACKGROUND_COLOR);
    this->display.getTextBounds(text, 0, y, &boundsX, &boundsY, &width, &height);
    this->display.setCursor((this->display.width() - width) / 2, y);
    this->display.print(text);
}

void St7789Display::formatAge(
    const SensorDisplayPage& page,
    char* output,
    size_t size
) const {
    if (page.ageMinutes == 0) {
        std::snprintf(output, size, "UPDATED NOW");
    } else if (page.ageMinutes < 60) {
        std::snprintf(output, size, "%lu MIN AGO", static_cast<unsigned long>(page.ageMinutes));
    } else if (page.ageMinutes < 1440) {
        std::snprintf(
            output,
            size,
            "%luH %luM AGO",
            static_cast<unsigned long>(page.ageMinutes / 60),
            static_cast<unsigned long>(page.ageMinutes % 60)
        );
    } else {
        const uint32_t days = (page.ageMinutes + 720) / 1440;
        std::snprintf(output, size, "%luD AGO", static_cast<unsigned long>(days));
    }
}
