#include "infrastructure/actuators/ExternalLedActuator.h"

ExternalLedActuator::ExternalLedActuator(int pin) : pin(pin) {}

void ExternalLedActuator::begin() {
    pinMode(pin, OUTPUT);
    setState(false);
}

void ExternalLedActuator::setState(bool state) {
    digitalWrite(pin, state ? HIGH : LOW);
}
