#include "application/display/SensorDisplayController.h"

namespace {
    struct PageDefinition {
        SensorType type;
        const char* label;
        const char* unit;
        uint8_t decimalPlaces;
    };

    const PageDefinition PAGES[] = {
        {SensorType::HOUSE_TEMP, "HOUSE", "C", 1},
        {SensorType::OUTDOOR_TEMP, "OUTSIDE", "C", 1},
        {SensorType::WATER_TEMP, "SHOWER", "C", 1},
        {SensorType::WATER_LEVEL_LITER, "WATER", "L", 0},
        {SensorType::BATTERY_VOLTAGE, "BATTERY VOLT", "V", 1},
        {SensorType::BATTERY_PERCENT, "BATTERY LEVEL", "%", 0},
    };

    const uint8_t PAGE_COUNT = sizeof(PAGES) / sizeof(PAGES[0]);
    const uint32_t STALE_AFTER_MINUTES = 60;
}

SensorDisplayController::SensorDisplayController(
    const SensorData& sensorData,
    const MonotonicClock& clock,
    SensorDisplayView& view,
    uint32_t pageDurationMs
) : sensorData(sensorData),
    clock(clock),
    view(view),
    pageDurationMs(pageDurationMs),
    lastPageChangeMs(0),
    lastRenderedUpdateMs(0),
    currentPageIndex(0),
    hasCurrentPage(false)
{}

void SensorDisplayController::begin() {
    this->view.begin();
    this->lastPageChangeMs = this->clock.now();
    this->showNextFreshPage(0, this->lastPageChangeMs);
}

void SensorDisplayController::update() {
    const uint32_t nowMs = this->clock.now();

    if (!this->hasCurrentPage) {
        uint8_t nextPageIndex;
        if (this->findNextFreshPage(0, nowMs, nextPageIndex)) {
            this->currentPageIndex = nextPageIndex;
            this->hasCurrentPage = true;
            this->lastPageChangeMs = nowMs;
            this->renderCurrentPage(nowMs);
        }
        return;
    }

    if (!this->isFresh(this->currentPageIndex, nowMs)) {
        this->lastPageChangeMs = nowMs;
        this->showNextFreshPage(this->currentPageIndex + 1, nowMs);
        return;
    }

    if (nowMs - this->lastPageChangeMs >= this->pageDurationMs) {
        this->lastPageChangeMs = nowMs;
        this->showNextFreshPage(this->currentPageIndex + 1, nowMs);
        return;
    }

    const SensorType currentType = PAGES[this->currentPageIndex].type;
    const uint32_t updatedAtMs = this->sensorData.getUpdatedAtMs(currentType);
    if (updatedAtMs != this->lastRenderedUpdateMs) {
        this->renderCurrentPage(nowMs);
    }
}

bool SensorDisplayController::isFresh(uint8_t pageIndex, uint32_t nowMs) const {
    const SensorType type = PAGES[pageIndex].type;
    return this->sensorData.hasKey(type)
        && (nowMs - this->sensorData.getUpdatedAtMs(type)) / 60000UL < STALE_AFTER_MINUTES;
}

bool SensorDisplayController::findNextFreshPage(
    uint8_t startIndex,
    uint32_t nowMs,
    uint8_t& result
) const {
    for (uint8_t offset = 0; offset < PAGE_COUNT; offset++) {
        const uint8_t pageIndex = (startIndex + offset) % PAGE_COUNT;
        if (this->isFresh(pageIndex, nowMs)) {
            result = pageIndex;
            return true;
        }
    }

    return false;
}

void SensorDisplayController::showNextFreshPage(
    uint8_t startIndex,
    uint32_t nowMs
) {
    uint8_t nextPageIndex;
    if (!this->findNextFreshPage(startIndex, nowMs, nextPageIndex)) {
        this->hasCurrentPage = false;
        this->lastRenderedUpdateMs = 0;
        this->view.showNoFreshData();
        return;
    }

    this->currentPageIndex = nextPageIndex;
    this->hasCurrentPage = true;
    this->renderCurrentPage(nowMs);
}

void SensorDisplayController::renderCurrentPage(uint32_t nowMs) {
    const PageDefinition& definition = PAGES[this->currentPageIndex];
    const uint32_t updatedAtMs = this->sensorData.getUpdatedAtMs(definition.type);
    const uint32_t ageMinutes = (nowMs - updatedAtMs) / 60000UL;

    this->lastRenderedUpdateMs = updatedAtMs;

    this->view.show({
        definition.label,
        definition.unit,
        this->sensorData.getValue(definition.type),
        ageMinutes,
        definition.decimalPlaces,
    });
}
