#include "presentation/WebServer.h"
#include <cstdlib>

WebServer::WebServer(
    WebSocket& webSocket,
    FileSystem& fileSystem,
    SensorUpdateService& sensorUpdateService
) : server(80),
    webSocket(webSocket),
    fileSystem(fileSystem),
    sensorUpdateService(sensorUpdateService)
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
        || !this->sensorUpdateService.updateValue(SensorType::HOUSE_TEMP, temperature)
    ) {
        request->send(400, "text/plain", "Missing or invalid temperature value");
        return;
    }

    request->send(200, "text/plain", "Temperature received");
}

void WebServer::handleOutdoorTemperature(AsyncWebServerRequest* request) {
    float temperature;
    if (
        !this->parseFloatParam(request, TEMPERATURE_OUTDOOR_PARAM_NAME, temperature)
        || !this->sensorUpdateService.updateValue(SensorType::OUTDOOR_TEMP, temperature)
    ) {
        request->send(400, "text/plain", "Missing or invalid temperature value");
        return;
    }

    request->send(200, "text/plain", "Temperature received");
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
        && this->sensorUpdateService.updateShower(temperature, liters, voltage, percent);

    if (!isValid) {
        request->send(400, "text/plain", "Missing or invalid shower data");
        return;
    }

    request->send(200, "text/plain", "Data received");
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
