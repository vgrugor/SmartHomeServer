#include "presentation/WebServer.h"

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

    server.serveStatic("/", LittleFS, "/");

    server.on("/", HTTP_GET, [this](AsyncWebServerRequest* request) {
        this->handleRoot(request);
    });

    server.on("/house/temperature", HTTP_GET, [this](AsyncWebServerRequest* request) {
        this->handleHouseTemperature(request);
    });

    server.on("/outdoor/temperature", HTTP_GET, [this](AsyncWebServerRequest* request) {
        this->handleOutdoorTemperature(request);
    });

    server.on("/shower/update", HTTP_GET, [this](AsyncWebServerRequest* request) {
        this->handleShowerUpdate(request);
    });

    server.begin();
}

void WebServer::handleRoot(AsyncWebServerRequest* request) {
    request->send(LittleFS, "/index.html", "text/html");
}

void WebServer::handleHouseTemperature(AsyncWebServerRequest* request) {
    if (request->hasParam(TEMPERATURE_HOUSE_PARAM_NAME)) {
        const AsyncWebParameter* temp = request->getParam(TEMPERATURE_HOUSE_PARAM_NAME);

        this->sensorData.setValue(SensorType::HOUSE_TEMP, temp->value().toFloat());

        request->send(200, "text/plain", "Temperature received");

        EventNotifier::getInstance().notifyObservers(EventType::WEB_SOCKET_NOTIFY_CLIENT);
    } else {
        request->send(400, "text/plain", "Missing temperature value");
    }
}

void WebServer::handleOutdoorTemperature(AsyncWebServerRequest* request) {
    if (request->hasParam(TEMPERATURE_OUTDOOR_PARAM_NAME)) {
        const AsyncWebParameter* temp = request->getParam(TEMPERATURE_OUTDOOR_PARAM_NAME);

        this->sensorData.setValue(SensorType::OUTDOOR_TEMP, temp->value().toFloat());

        request->send(200, "text/plain", "Temperature received");

        EventNotifier::getInstance().notifyObservers(EventType::WEB_SOCKET_NOTIFY_CLIENT);
    } else {
        request->send(400, "text/plain", "Missing temperature value");
    }
}

void WebServer::handleShowerUpdate(AsyncWebServerRequest* request) {
    if (
        request->hasParam(TEMPERATURE_WATER_SHOWER_PARAM_NAME)
        && request->hasParam(VOLUME_WATER_SHOWER_PARAM_NAME)
        && request->hasParam(BATTERY_VOLTAGE_SHOWER_PARAM_NAME)
        && request->hasParam(BATTERY_PERCENT_SHOWER_PARAM_NAME)
    ) {
        const AsyncWebParameter* temp = request->getParam(TEMPERATURE_WATER_SHOWER_PARAM_NAME);
        const AsyncWebParameter* liter = request->getParam(VOLUME_WATER_SHOWER_PARAM_NAME);
        const AsyncWebParameter* voltage = request->getParam(BATTERY_VOLTAGE_SHOWER_PARAM_NAME);
        const AsyncWebParameter* percent = request->getParam(BATTERY_PERCENT_SHOWER_PARAM_NAME);

        this->sensorData.setValue(SensorType::WATER_TEMP, temp->value().toFloat());
        this->sensorData.setValue(SensorType::WATER_LEVEL_LITER, liter->value().toFloat());
        this->sensorData.setValue(SensorType::BATTERY_VOLTAGE, voltage->value().toFloat());
        this->sensorData.setValue(SensorType::BATTERY_PERCENT, percent->value().toFloat());

        request->send(200, "text/plain", "Data received");

        EventNotifier::getInstance().notifyObservers(EventType::WEB_SOCKET_NOTIFY_CLIENT);
    } else {
        request->send(400, "text/plain", "Missing temperature value");
    }
}

void WebServer::handleClient() {}
