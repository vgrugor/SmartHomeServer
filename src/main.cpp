#include <Arduino.h>
#include "infrastructure/env.h"
#include "infrastructure/wifi/WiFiManager.h"
#include "infrastructure/fs/FileSystem.h"
#include "application/WSDataTransformer.h"
#include "application/WsMessageHandler.h"
#include "infrastructure/web/WebSocket.h"
#include "presentation/WebServer.h"
#include "infrastructure/actuators/BuzzerActuator.h"
#include "infrastructure/actuators/ExternalLedActuator.h"
#include "presentation/EventNotifier.h"
#include "presentation/observers/BuzzerObserver.h"
#include "presentation/observers/LedObserver.h"
#include "presentation/observers/SerialObserver.h"
#include "presentation/observers/WebSocketObserver.h"
#include "infrastructure/loaders/OTALoader.h"
#include "domain/SensorData.h"

WiFiManager wifiManager(WIFI_SSID, WIFI_PASSWORD, WIFI_IP, WIFI_GATEWAY, WIFI_SUBNET);
FileSystem fileSystem;

SensorData sensorData;

WsDataTransformer wsDataTransformer(sensorData);
WsMessageHandler wsMessageHandler;
WebSocket webSocket(wsMessageHandler, wsDataTransformer);
WebServer webServer(webSocket, fileSystem, sensorData);

ExternalLedActuator externalLedActuator(GREEN_LED_PIN);
BuzzerActuator buzzerActuator(BUZZER_PIN);

LedObserver ledObserver(externalLedActuator);
BuzzerObserver buzzerObserver(buzzerActuator);
SerialObserver serialObserver;
WebSocketObserver webSocketObserver(webSocket);

EventNotifier& eventNotifier = EventNotifier::getInstance();

OTALoader OTA(OTA_HOSTNAME, OTA_PASSWORD);

void setup() {
    Serial.begin(115200);

    eventNotifier.addObserver(&ledObserver);
    eventNotifier.addObserver(&buzzerObserver);
    eventNotifier.addObserver(&serialObserver);
    eventNotifier.addObserver(&webSocketObserver);

    wifiManager.connect();

    webServer.begin();

    OTA.begin();
}

void loop() {
    wifiManager.reconnect();

    webServer.handleClient();

    OTA.handle();
}
