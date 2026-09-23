#include "presentation/SensorUpdateEventNotifier.h"
#include <Arduino.h>
#include "application/events/EventNotifier.h"

void SensorUpdateEventNotifier::onSensorValueSaved(SensorType type, float value) {
    const String valueString = String(value, 2);
    String message;

    switch (type) {
        case SensorType::HOUSE_TEMP:
            message = "Set house temperature: " + valueString + "°C";
            break;
        case SensorType::OUTDOOR_TEMP:
            message = "Set outdoor temperature: " + valueString + "°C";
            break;
        case SensorType::WATER_TEMP:
            message = "Set shower water temperature: " + valueString + "°C";
            break;
        case SensorType::WATER_LEVEL_LITER:
            message = "Set shower water level liters: " + valueString + "l";
            break;
        case SensorType::BATTERY_VOLTAGE:
            message = "Set shower battery voltage: " + valueString + "v";
            break;
        case SensorType::BATTERY_PERCENT:
            message = "Set battery level percent: " + valueString + "%";
            break;
    }

    EventNotifier::getInstance().notifyObservers(EventType::PARAM_SAVED, message.c_str());
}

void SensorUpdateEventNotifier::onSensorValuesUpdated() {
    EventNotifier::getInstance().notifyObservers(EventType::WEB_SOCKET_NOTIFY_CLIENT);
}
