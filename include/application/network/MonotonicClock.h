#ifndef MONOTONIC_CLOCK_H
#define MONOTONIC_CLOCK_H

#include <cstdint>

class MonotonicClock {
    public:
        virtual ~MonotonicClock() = default;
        virtual uint32_t now() const = 0;
};

#endif // MONOTONIC_CLOCK_H
