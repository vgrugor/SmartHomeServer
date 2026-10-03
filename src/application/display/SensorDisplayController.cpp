#include "application/display/SensorDisplayController.h"

namespace {
    const uint8_t PAGE_COUNT = 2;
    const uint32_t STALE_AFTER_MS = 60UL * 60000UL;

    bool sameReading(const DisplayReading& left, const DisplayReading& right) {
        return left.available == right.available
            && (!left.available || (left.value == right.value
                && left.ageMinutes == right.ageMinutes));
    }

    bool samePage(const SensorDisplayPage& left, const SensorDisplayPage& right) {
        return left.kind == right.kind
            && sameReading(left.houseTemperature, right.houseTemperature)
            && sameReading(left.outdoorTemperature, right.outdoorTemperature)
            && sameReading(left.showerTemperature, right.showerTemperature)
            && sameReading(left.showerVolume, right.showerVolume)
            && sameReading(left.batteryVoltage, right.batteryVoltage)
            && sameReading(left.batteryPercent, right.batteryPercent);
    }
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
    currentPageIndex(0),
    hasCurrentPage(false),
    showingNoData(false),
    hasRenderedPage(false),
    lastRenderedPage({})
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
            this->renderCurrentPage(nowMs, true);
        }
        return;
    }

    if (!this->isPageAvailable(this->currentPageIndex, nowMs)) {
        this->lastPageChangeMs = nowMs;
        this->showNextFreshPage(this->currentPageIndex + 1, nowMs);
        return;
    }

    if (nowMs - this->lastPageChangeMs >= this->pageDurationMs) {
        this->lastPageChangeMs = nowMs;
        this->showNextFreshPage(this->currentPageIndex + 1, nowMs);
        return;
    }

    this->renderCurrentPage(nowMs);
}

bool SensorDisplayController::isReadingFresh(SensorType type, uint32_t nowMs) const {
    return this->sensorData.hasKey(type)
        && nowMs - this->sensorData.getUpdatedAtMs(type) < STALE_AFTER_MS;
}

bool SensorDisplayController::isPageAvailable(uint8_t pageIndex, uint32_t nowMs) const {
    if (pageIndex == 0) {
        return this->isReadingFresh(SensorType::HOUSE_TEMP, nowMs)
            || this->isReadingFresh(SensorType::OUTDOOR_TEMP, nowMs);
    }

    // A shower update supplies all four values together. Never show a partial batch.
    return this->isReadingFresh(SensorType::WATER_TEMP, nowMs)
        && this->isReadingFresh(SensorType::WATER_LEVEL_LITER, nowMs)
        && this->isReadingFresh(SensorType::BATTERY_VOLTAGE, nowMs)
        && this->isReadingFresh(SensorType::BATTERY_PERCENT, nowMs);
}

bool SensorDisplayController::findNextFreshPage(
    uint8_t startIndex,
    uint32_t nowMs,
    uint8_t& result
) const {
    for (uint8_t offset = 0; offset < PAGE_COUNT; offset++) {
        const uint8_t pageIndex = (startIndex + offset) % PAGE_COUNT;
        if (this->isPageAvailable(pageIndex, nowMs)) {
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
        this->hasRenderedPage = false;
        if (!this->showingNoData) {
            this->view.showNoFreshData();
            this->showingNoData = true;
        }
        return;
    }

    const bool pageChanged = !this->hasCurrentPage
        || this->currentPageIndex != nextPageIndex;
    this->currentPageIndex = nextPageIndex;
    this->hasCurrentPage = true;
    this->renderCurrentPage(nowMs, pageChanged || this->showingNoData);
}

DisplayReading SensorDisplayController::readingFor(SensorType type, uint32_t nowMs) const {
    DisplayReading reading = {};
    reading.available = this->isReadingFresh(type, nowMs);
    if (reading.available) {
        reading.value = this->sensorData.getValue(type);
        reading.ageMinutes = (nowMs - this->sensorData.getUpdatedAtMs(type)) / 60000UL;
    }
    return reading;
}

SensorDisplayPage SensorDisplayController::makePage(
    uint8_t pageIndex,
    uint32_t nowMs
) const {
    SensorDisplayPage page = {};
    if (pageIndex == 0) {
        page.kind = SensorDisplayPageKind::TEMPERATURES;
        page.houseTemperature = this->readingFor(SensorType::HOUSE_TEMP, nowMs);
        page.outdoorTemperature = this->readingFor(SensorType::OUTDOOR_TEMP, nowMs);
    } else {
        page.kind = SensorDisplayPageKind::SHOWER;
        page.showerTemperature = this->readingFor(SensorType::WATER_TEMP, nowMs);
        page.showerVolume = this->readingFor(SensorType::WATER_LEVEL_LITER, nowMs);
        page.batteryVoltage = this->readingFor(SensorType::BATTERY_VOLTAGE, nowMs);
        page.batteryPercent = this->readingFor(SensorType::BATTERY_PERCENT, nowMs);
    }
    return page;
}

void SensorDisplayController::renderCurrentPage(uint32_t nowMs, bool force) {
    const SensorDisplayPage page = this->makePage(this->currentPageIndex, nowMs);
    if (force || !this->hasRenderedPage || !samePage(page, this->lastRenderedPage)) {
        this->view.show(page);
        this->lastRenderedPage = page;
        this->hasRenderedPage = true;
    }
    this->showingNoData = false;
}
