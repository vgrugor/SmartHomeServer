#ifndef LOCAL_DATE_TIME_H
#define LOCAL_DATE_TIME_H

struct LocalDateTime {
    int year;
    int month;
    int day;
    int hour;
    int minute;

    int dateKey() const {
        return year * 10000 + month * 100 + day;
    }
};

class LocalDateTimeSource {
    public:
        virtual ~LocalDateTimeSource() = default;
        virtual bool now(LocalDateTime& result) const = 0;
};

#endif // LOCAL_DATE_TIME_H
