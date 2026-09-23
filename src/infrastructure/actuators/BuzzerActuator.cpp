#include "infrastructure/actuators/BuzzerActuator.h"

BuzzerActuator::BuzzerActuator(int pin) : pin(pin) {}

void BuzzerActuator::begin() {
    pinMode(pin, OUTPUT);
    setState(false);
}

void BuzzerActuator::setState(bool state) {
    digitalWrite(pin, state ? HIGH : LOW);
}
