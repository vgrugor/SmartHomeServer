#include <Arduino.h>
#include "config/DeviceConfig.h"
#include "config/Secrets.h"
#include "application/network/WiFiConnectionManager.h"
#include "infrastructure/wifi/ArduinoClock.h"
#include "infrastructure/wifi/EspWiFiConnection.h"
#include "infrastructure/fs/FileSystem.h"
#include "application/WSDataTransformer.h"
#include "application/WsMessageHandler.h"
#include "application/SensorUpdateService.h"
#include "application/SensorValueValidator.h"
#include "application/display/SensorDisplayController.h"
#include "infrastructure/web/WebSocket.h"
#include "presentation/WebServer.h"
#include "presentation/SensorUpdateEventNotifier.h"
#include "infrastructure/actuators/BuzzerActuator.h"
#include "infrastructure/actuators/ExternalLedActuator.h"
#include "infrastructure/display/St7789Display.h"
#include "application/events/EventNotifier.h"
#include "presentation/observers/BuzzerObserver.h"
#include "presentation/observers/LedObserver.h"
#include "presentation/observers/SerialObserver.h"
#include "presentation/observers/WebSocketObserver.h"
#include "infrastructure/loaders/OTALoader.h"
#include "domain/SensorData.h"

EventNotifier& eventNotifier = EventNotifier::getInstance();

EspWiFiConnection wifiConnection(
    WIFI_SSID,
    WIFI_PASSWORD,
    WIFI_IP,
    WIFI_GATEWAY,
    WIFI_SUBNET
);
ArduinoClock systemClock;
WiFiConnectionManager wifiManager(wifiConnection, systemClock, eventNotifier);
FileSystem fileSystem;

SensorData sensorData;
SensorValueValidator sensorValueValidator;
SensorUpdateEventNotifier sensorUpdateEventNotifier;
SensorUpdateService sensorUpdateService(
    sensorData,
    sensorValueValidator,
    sensorUpdateEventNotifier,
    systemClock
);

WsDataTransformer wsDataTransformer(sensorData, systemClock);
WsMessageHandler wsMessageHandler;
WebSocket webSocket(wsMessageHandler, wsDataTransformer);
WebServer webServer(webSocket, fileSystem, sensorUpdateService);

ExternalLedActuator externalLedActuator(GREEN_LED_PIN);
BuzzerActuator buzzerActuator(BUZZER_PIN);
St7789Display dashboardDisplay(
    TFT_CHIP_SELECT_PIN,
    TFT_DATA_COMMAND_PIN,
    TFT_RESET_PIN
);
SensorDisplayController displayController(
    sensorData,
    systemClock,
    dashboardDisplay,
    TFT_PAGE_DURATION_MS
);

LedObserver ledObserver(externalLedActuator);
BuzzerObserver buzzerObserver(buzzerActuator);
SerialObserver serialObserver;
WebSocketObserver webSocketObserver(webSocket);

OTALoader OTA(OTA_HOSTNAME, OTA_PASSWORD);
bool networkServicesStarted = false;

void setup() {
    Serial.begin(115200);

    externalLedActuator.begin();
    buzzerActuator.begin();
    displayController.begin();

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
    displayController.update();

    if (wifiManager.isConnected() && !networkServicesStarted) {
        webServer.begin();
        OTA.begin();
        networkServicesStarted = true;
    }

    if (networkServicesStarted) {
        OTA.handle();
    }
}
