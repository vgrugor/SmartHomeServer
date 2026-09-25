#include "infrastructure/time/KyivDateTimeSource.h"
#include <Arduino.h>
#include <TZ.h>
#include <ctime>

void KyivDateTimeSource::begin() {
    // The ESP8266 core keeps the historical Kiev spelling for this Kyiv TZ rule.
    configTime(TZ_Europe_Kiev, "pool.ntp.org", "time.google.com");
}

bool KyivDateTimeSource::now(LocalDateTime& result) const {
    const time_t timestamp = time(nullptr);
    if (timestamp < 1704067200) { // 2024-01-01 UTC: NTP has not set a usable clock.
        return false;
    }

    struct tm local;
    if (localtime_r(&timestamp, &local) == nullptr) {
        return false;
    }

    result = {local.tm_year + 1900, local.tm_mon + 1, local.tm_mday,
              local.tm_hour, local.tm_min};
    return true;
}
