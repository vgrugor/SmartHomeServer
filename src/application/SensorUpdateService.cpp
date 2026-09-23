#include "application/SensorUpdateService.h"

SensorUpdateService::SensorUpdateService(
    SensorData& sensorData,
    const SensorValueValidator& validator,
    SensorUpdateListener& listener,
    const MonotonicClock& clock
) : sensorData(sensorData),
    validator(validator),
    listener(listener),
    clock(clock)
{}

bool SensorUpdateService::updateValue(SensorType type, float value) {
    if (!this->validator.isValid(type, value)) {
        return false;
    }

    this->saveValue(type, value);
    this->listener.onSensorValuesUpdated();
    return true;
}

bool SensorUpdateService::updateShower(
    float temperature,
    float liters,
    float voltage,
    float percent
) {
    const bool isValid =
        this->validator.isValid(SensorType::WATER_TEMP, temperature)
        && this->validator.isValid(SensorType::WATER_LEVEL_LITER, liters)
        && this->validator.isValid(SensorType::BATTERY_VOLTAGE, voltage)
        && this->validator.isValid(SensorType::BATTERY_PERCENT, percent);

    if (!isValid) {
        return false;
    }

    this->saveValue(SensorType::WATER_TEMP, temperature);
    this->saveValue(SensorType::WATER_LEVEL_LITER, liters);
    this->saveValue(SensorType::BATTERY_VOLTAGE, voltage);
    this->saveValue(SensorType::BATTERY_PERCENT, percent);
    this->listener.onSensorValuesUpdated();
    return true;
}

void SensorUpdateService::saveValue(SensorType type, float value) {
    this->sensorData.setValue(type, value, this->clock.now());
    this->listener.onSensorValueSaved(type, value);
}
