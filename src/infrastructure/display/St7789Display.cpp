#include "infrastructure/display/St7789Display.h"
#include <cstdio>
#include <cstring>

namespace {
    const uint16_t BACKGROUND_COLOR = ST77XX_BLACK;
    const uint16_t HOUSE_COLOR = ST77XX_CYAN;
    const uint16_t SUN_COLOR = ST77XX_YELLOW;
    const uint16_t VALUE_COLOR = ST77XX_WHITE;
    const uint16_t NO_DATA_COLOR = 0x7BEF;
    const uint16_t AGE_COLOR = 0xBDF7;
    const uint16_t SEPARATOR_COLOR = 0x4208;
}

St7789Display::St7789Display(
    int chipSelectPin,
    int dataCommandPin,
    int resetPin
) : display(chipSelectPin, dataCommandPin, resetPin)
{}

void St7789Display::begin() {
    this->display.init(240, 320);
    this->display.setRotation(0);
    this->display.setTextWrap(false);
    this->display.fillScreen(BACKGROUND_COLOR);
}

void St7789Display::show(const SensorDisplayPage& page) {
    this->display.fillScreen(BACKGROUND_COLOR);

    if (page.kind == SensorDisplayPageKind::TEMPERATURES) {
        this->drawTemperatureSection(page.houseTemperature, 0, true);
        this->display.drawFastHLine(8, 159, 224, SEPARATOR_COLOR);
        this->drawTemperatureSection(page.outdoorTemperature, 160, false);
        return;
    }

    this->drawCenteredText("SHOWER", 6, 3, HOUSE_COLOR);
    this->drawShowerRow("WATER TEMP", page.showerTemperature, "C", 1, 45);
    this->drawShowerRow("WATER", page.showerVolume, "L", 0, 113);
    this->drawShowerRow("BATTERY VOLT", page.batteryVoltage, "V", 1, 181);
    this->drawShowerRow("BATTERY LEVEL", page.batteryPercent, "%", 0, 249);
}

void St7789Display::showNoFreshData() {
    this->display.fillScreen(BACKGROUND_COLOR);
    this->drawCenteredText("NO FRESH", 112, 4, NO_DATA_COLOR);
    this->drawCenteredText("DATA", 168, 4, NO_DATA_COLOR);
}

void St7789Display::drawCenteredText(
    const char* text,
    int16_t y,
    uint8_t size,
    uint16_t color
) {
    this->drawTextInRegion(text, 0, this->display.width(), y, size, color);
}

void St7789Display::drawTextInRegion(
    const char* text,
    int16_t left,
    int16_t width,
    int16_t y,
    uint8_t maxSize,
    uint16_t color
) {
    uint8_t size = maxSize;
    while (size > 1 && std::strlen(text) * 6U * size > static_cast<unsigned>(width)) {
        size--;
    }

    int16_t boundsX;
    int16_t boundsY;
    uint16_t textWidth;
    uint16_t textHeight;
    this->display.setTextSize(size);
    this->display.setTextColor(color, BACKGROUND_COLOR);
    this->display.getTextBounds(text, 0, y, &boundsX, &boundsY, &textWidth, &textHeight);
    this->display.setCursor(left + (width - textWidth) / 2, y);
    this->display.print(text);
}

void St7789Display::drawHouseIcon(int16_t x, int16_t y, uint16_t color) {
    this->display.drawLine(x - 22, y, x, y - 18, color);
    this->display.drawLine(x, y - 18, x + 22, y, color);
    this->display.drawRect(x - 17, y, 35, 30, color);
    this->display.drawRect(x - 5, y + 13, 11, 17, color);
}

void St7789Display::drawSunIcon(int16_t x, int16_t y, uint16_t color) {
    this->display.drawCircle(x, y, 13, color);
    this->display.drawCircle(x, y, 12, color);
    this->display.drawLine(x - 24, y, x - 17, y, color);
    this->display.drawLine(x + 17, y, x + 24, y, color);
    this->display.drawLine(x, y - 24, x, y - 17, color);
    this->display.drawLine(x, y + 17, x, y + 24, color);
    this->display.drawLine(x - 17, y - 17, x - 13, y - 13, color);
    this->display.drawLine(x + 13, y + 13, x + 17, y + 17, color);
    this->display.drawLine(x - 17, y + 17, x - 13, y + 13, color);
    this->display.drawLine(x + 13, y - 13, x + 17, y - 17, color);
}

void St7789Display::drawTemperatureSection(
    const DisplayReading& reading,
    int16_t top,
    bool inside
) {
    const uint16_t iconColor = reading.available
        ? (inside ? HOUSE_COLOR : SUN_COLOR)
        : NO_DATA_COLOR;
    if (inside) {
        this->drawHouseIcon(35, top + 65, iconColor);
    } else {
        this->drawSunIcon(35, top + 65, iconColor);
    }

    char value[24];
    if (reading.available) {
        std::snprintf(value, sizeof(value), "%.1fC", static_cast<double>(reading.value));
    } else {
        std::snprintf(value, sizeof(value), "--");
    }
    this->drawTextInRegion(
        value, 68, 168, top + 47, 5,
        reading.available ? VALUE_COLOR : NO_DATA_COLOR
    );

    if (reading.available) {
        char age[24];
        this->formatAge(reading, age, sizeof(age));
        this->drawCenteredText(age, top + 120, 2, AGE_COLOR);
    }
}

void St7789Display::drawShowerRow(
    const char* label,
    const DisplayReading& reading,
    const char* unit,
    uint8_t decimals,
    int16_t top
) {
    this->display.drawFastHLine(8, top, 224, SEPARATOR_COLOR);
    this->drawCenteredText(label, top + 5, 2, HOUSE_COLOR);

    char value[32];
    std::snprintf(
        value, sizeof(value), "%.*f %s", decimals,
        static_cast<double>(reading.value), unit
    );
    this->drawCenteredText(value, top + 27, 4, VALUE_COLOR);
}

void St7789Display::formatAge(
    const DisplayReading& reading,
    char* output,
    size_t size
) const {
    if (reading.ageMinutes == 0) {
        std::snprintf(output, size, "UPDATED NOW");
    } else {
        std::snprintf(
            output, size, "%lu MIN AGO",
            static_cast<unsigned long>(reading.ageMinutes)
        );
    }
}
