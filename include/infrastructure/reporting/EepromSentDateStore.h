#ifndef EEPROM_SENT_DATE_STORE_H
#define EEPROM_SENT_DATE_STORE_H

#include "application/reporting/DailyTemperatureReporter.h"

class EepromSentDateStore : public SentDateStore {
    public:
        bool load(int& dateKey) override;
        bool save(int dateKey) override;
};

#endif // EEPROM_SENT_DATE_STORE_H
