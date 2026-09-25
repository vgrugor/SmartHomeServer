#ifndef DAILY_TEMPERATURE_REPORTER_H
#define DAILY_TEMPERATURE_REPORTER_H

#include <cstdint>
#include "application/network/MonotonicClock.h"
#include "application/reporting/LocalDateTime.h"
#include "domain/SensorData.h"

class TemperatureMessageSender {
    public:
        virtual ~TemperatureMessageSender() = default;
        virtual bool send(int dateKey, int hour, int minute, float houseC, float outdoorC) = 0;
        virtual int lastTransportCode() const { return 0; }
};

class SentDateStore {
    public:
        virtual ~SentDateStore() = default;
        virtual bool load(int& dateKey) = 0;
        virtual bool save(int dateKey) = 0;
};

class DailyTemperatureReporter {
    public:
        DailyTemperatureReporter(
            const SensorData& sensorData,
            const LocalDateTimeSource& localTime,
            const MonotonicClock& clock,
            TemperatureMessageSender& sender,
            SentDateStore& store
        );

        bool begin();
        void update(bool networkConnected);
        int getLastSentDate() const;
        bool isReady() const;
        bool hasPendingSave() const;
        bool hasAttemptedSend() const;
        int getLastAttemptDate() const;
        bool lastAttemptSucceeded() const;
        bool getLocalNow(LocalDateTime& result) const;
        bool hasBothReadings() const;
        int getLastTransportCode() const;

    private:
        static const uint32_t RETRY_INTERVAL_MS = 5UL * 60UL * 1000UL;

        const SensorData& sensorData;
        const LocalDateTimeSource& localTime;
        const MonotonicClock& clock;
        TemperatureMessageSender& sender;
        SentDateStore& store;
        int lastSentDate;
        uint32_t lastAttemptAtMs;
        bool ready;
        bool hasAttempted;
        bool pendingSave;
        int lastAttemptDate;
        bool lastSendSucceeded;
};

#endif // DAILY_TEMPERATURE_REPORTER_H
