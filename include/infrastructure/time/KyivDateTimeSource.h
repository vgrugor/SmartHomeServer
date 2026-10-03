#ifndef KYIV_DATE_TIME_SOURCE_H
#define KYIV_DATE_TIME_SOURCE_H

#include "application/reporting/LocalDateTime.h"

class KyivDateTimeSource : public LocalDateTimeSource {
    public:
        void begin();
        bool now(LocalDateTime& result) const override;
};

#endif // KYIV_DATE_TIME_SOURCE_H
