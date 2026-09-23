#include "infrastructure/wifi/ArduinoClock.h"
#include <Arduino.h>

uint32_t ArduinoClock::now() const {
    return static_cast<uint32_t>(millis());
}
