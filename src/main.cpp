#include <Arduino.h>
#include "config/DeviceConfig.h"
#include "config/Secrets.h"
#include "infrastructure/wifi/WiFiManager.h"
#include "infrastructure/fs/FileSystem.h"
#include "application/WSDataTransformer.h"
#include "application/WsMessageHandler.h"
#include "infrastructure/web/WebSocket.h"
#include "presentation/WebServer.h"
#include "infrastructure/actuators/BuzzerActuator.h"
#include "infrastructure/actuators/ExternalLedActuator.h"
#include "application/events/EventNotifier.h"
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
bool networkServicesStarted = false;

void setup() {
    Serial.begin(115200);

    externalLedActuator.begin();
    buzzerActuator.begin();

    if (!fileSystem.begin()) {
        Serial.println("Failed to mount LittleFS");
    }

    eventNotifier.addObserver(&ledObserver);
    eventNotifier.addObserver(&buzzerObserver);
    eventNotifier.addObserver(&serialObserver);
    eventNotifier.addObserver(&webSocketObserver);

    wifiManager.begin();
}

void loop() {
    wifiManager.update();

    if (wifiManager.isConnected() && !networkServicesStarted) {
        webServer.begin();
        OTA.begin();
        networkServicesStarted = true;
    }

    if (networkServicesStarted) {
        OTA.handle();
    }
}
