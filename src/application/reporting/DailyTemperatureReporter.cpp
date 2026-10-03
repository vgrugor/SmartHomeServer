#include "application/reporting/DailyTemperatureReporter.h"

DailyTemperatureReporter::DailyTemperatureReporter(
    const SensorData& sensorData,
    const LocalDateTimeSource& localTime,
    const MonotonicClock& clock,
    TemperatureMessageSender& sender,
    SentDateStore& store
) : sensorData(sensorData),
    localTime(localTime),
    clock(clock),
    sender(sender),
    store(store),
    lastSentDate(0),
    lastAttemptAtMs(0),
    ready(false),
    hasAttempted(false),
    pendingSave(false),
    lastAttemptDate(0),
    lastSendSucceeded(false)
{}

bool DailyTemperatureReporter::begin() {
    this->ready = this->store.load(this->lastSentDate);
    return this->ready;
}

int DailyTemperatureReporter::getLastSentDate() const {
    return this->lastSentDate;
}

bool DailyTemperatureReporter::isReady() const {
    return this->ready;
}

bool DailyTemperatureReporter::hasPendingSave() const {
    return this->pendingSave;
}

bool DailyTemperatureReporter::hasAttemptedSend() const {
    return this->lastAttemptDate != 0;
}

int DailyTemperatureReporter::getLastAttemptDate() const {
    return this->lastAttemptDate;
}

bool DailyTemperatureReporter::lastAttemptSucceeded() const {
    return this->lastSendSucceeded;
}

bool DailyTemperatureReporter::getLocalNow(LocalDateTime& result) const {
    return this->localTime.now(result);
}

bool DailyTemperatureReporter::hasBothReadings() const {
    return this->sensorData.hasKey(SensorType::HOUSE_TEMP)
        && this->sensorData.hasKey(SensorType::OUTDOOR_TEMP);
}

int DailyTemperatureReporter::getLastTransportCode() const {
    return this->sender.lastTransportCode();
}

void DailyTemperatureReporter::update(bool networkConnected) {
    if (!this->ready) {
        return;
    }

    const uint32_t nowMs = this->clock.now();

    if (this->pendingSave) {
        if (!this->hasAttempted || nowMs - this->lastAttemptAtMs >= RETRY_INTERVAL_MS) {
            this->lastAttemptAtMs = nowMs;
            this->hasAttempted = true;
            this->pendingSave = !this->store.save(this->lastSentDate);
        }
        return;
    }

    if (!networkConnected) {
        return;
    }

    LocalDateTime dateTime;
    if (!this->localTime.now(dateTime) || dateTime.hour < 15) {
        return;
    }

    const int today = dateTime.dateKey();
    if (today <= this->lastSentDate || !this->hasBothReadings()) {
        return;
    }

    if (this->hasAttempted && nowMs - this->lastAttemptAtMs < RETRY_INTERVAL_MS) {
        return;
    }

    this->lastAttemptAtMs = nowMs;
    this->hasAttempted = true;

    this->lastAttemptDate = today;
    this->lastSendSucceeded = this->sender.send(
        today, dateTime.hour, dateTime.minute,
        this->sensorData.getValue(SensorType::HOUSE_TEMP),
        this->sensorData.getValue(SensorType::OUTDOOR_TEMP)
    );
    if (!this->lastSendSucceeded) {
        return;
    }

    this->lastSentDate = today;
    this->pendingSave = !this->store.save(today);
}
