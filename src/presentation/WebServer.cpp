#include "presentation/WebServer.h"
#include <cstdlib>

WebServer::WebServer(
    WebSocket& webSocket,
    FileSystem& fileSystem,
    SensorData& sensorData
) : server(80),
    webSocket(webSocket),
    fileSystem(fileSystem),
    sensorData(sensorData)
{}

void WebServer::begin() {
    server.addHandler(webSocket.getWebSocketObject());

    server.serveStatic("/", this->fileSystem.get(), "/");

    server.on("/", HTTP_GET, [this](AsyncWebServerRequest* request) {
        this->handleRoot(request);
    });

    server.on("/", HTTP_HEAD, [this](AsyncWebServerRequest* request) {
       request->send(200);
    });

    server.on("/house/temperature", HTTP_GET, [this](AsyncWebServerRequest* request) {
        this->handleHouseTemperature(request);
    });
    server.on("/house/temperature", HTTP_POST, [this](AsyncWebServerRequest* request) {
        this->handleHouseTemperature(request);
    });

    server.on("/outdoor/temperature", HTTP_GET, [this](AsyncWebServerRequest* request) {
        this->handleOutdoorTemperature(request);
    });
    server.on("/outdoor/temperature", HTTP_POST, [this](AsyncWebServerRequest* request) {
        this->handleOutdoorTemperature(request);
    });

    server.on("/shower/update", HTTP_GET, [this](AsyncWebServerRequest* request) {
        this->handleShowerUpdate(request);
    });
    server.on("/shower/update", HTTP_POST, [this](AsyncWebServerRequest* request) {
        this->handleShowerUpdate(request);
    });

    server.begin();
}

void WebServer::handleRoot(AsyncWebServerRequest* request) {
    request->send(this->fileSystem.get(), "/index.html", "text/html");
}

void WebServer::handleHouseTemperature(AsyncWebServerRequest* request) {
    float temperature;
    if (
        !this->parseFloatParam(request, TEMPERATURE_HOUSE_PARAM_NAME, temperature)
        || !this->sensorValueValidator.isValid(SensorType::HOUSE_TEMP, temperature)
    ) {
        request->send(400, "text/plain", "Missing or invalid temperature value");
        return;
    }

    this->saveSensorValue(SensorType::HOUSE_TEMP, temperature);
    request->send(200, "text/plain", "Temperature received");
    this->notifyClients();
}

void WebServer::handleOutdoorTemperature(AsyncWebServerRequest* request) {
    float temperature;
    if (
        !this->parseFloatParam(request, TEMPERATURE_OUTDOOR_PARAM_NAME, temperature)
        || !this->sensorValueValidator.isValid(SensorType::OUTDOOR_TEMP, temperature)
    ) {
        request->send(400, "text/plain", "Missing or invalid temperature value");
        return;
    }

    this->saveSensorValue(SensorType::OUTDOOR_TEMP, temperature);
    request->send(200, "text/plain", "Temperature received");
    this->notifyClients();
}

void WebServer::handleShowerUpdate(AsyncWebServerRequest* request) {
    float temperature;
    float liters;
    float voltage;
    float percent;

    const bool isValid =
        this->parseFloatParam(request, TEMPERATURE_WATER_SHOWER_PARAM_NAME, temperature)
        && this->parseFloatParam(request, VOLUME_WATER_SHOWER_PARAM_NAME, liters)
        && this->parseFloatParam(request, BATTERY_VOLTAGE_SHOWER_PARAM_NAME, voltage)
        && this->parseFloatParam(request, BATTERY_PERCENT_SHOWER_PARAM_NAME, percent)
        && this->sensorValueValidator.isValid(SensorType::WATER_TEMP, temperature)
        && this->sensorValueValidator.isValid(SensorType::WATER_LEVEL_LITER, liters)
        && this->sensorValueValidator.isValid(SensorType::BATTERY_VOLTAGE, voltage)
        && this->sensorValueValidator.isValid(SensorType::BATTERY_PERCENT, percent);

    if (!isValid) {
        request->send(400, "text/plain", "Missing or invalid shower data");
        return;
    }

    // Commit the complete update only after all fields have passed validation.
    this->saveSensorValue(SensorType::WATER_TEMP, temperature);
    this->saveSensorValue(SensorType::WATER_LEVEL_LITER, liters);
    this->saveSensorValue(SensorType::BATTERY_VOLTAGE, voltage);
    this->saveSensorValue(SensorType::BATTERY_PERCENT, percent);

    request->send(200, "text/plain", "Data received");
    this->notifyClients();
}

bool WebServer::parseFloatParam(
    AsyncWebServerRequest* request,
    const String& name,
    float& value
) const {
    const AsyncWebParameter* parameter = nullptr;

    if (request->hasParam(name)) {
        parameter = request->getParam(name);
    } else if (request->hasParam(name, true)) {
        parameter = request->getParam(name, true);
    } else {
        return false;
    }

    const String rawValue = parameter->value();
    if (rawValue.length() == 0) {
        return false;
    }

    char* end = nullptr;
    const char* start = rawValue.c_str();
    value = strtof(start, &end);

    return end != start && *end == '\0';
}

void WebServer::saveSensorValue(SensorType type, float value) {
    this->sensorData.setValue(type, value);

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

    EventNotifier::getInstance().notifyObservers(EventType::PARAM_SAVED, message);
}

void WebServer::notifyClients() {
    EventNotifier::getInstance().notifyObservers(EventType::WEB_SOCKET_NOTIFY_CLIENT);
}
