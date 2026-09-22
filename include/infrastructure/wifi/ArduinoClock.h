#ifndef ARDUINO_CLOCK_H
#define ARDUINO_CLOCK_H

#include "application/network/MonotonicClock.h"

class ArduinoClock : public MonotonicClock {
    public:
        uint32_t now() const override;
};

#endif // ARDUINO_CLOCK_H
