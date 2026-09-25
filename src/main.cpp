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
#include "infrastructure/web/WebSocket.h"
#include "presentation/WebServer.h"
#include "presentation/SensorUpdateEventNotifier.h"
#include "infrastructure/actuators/BuzzerActuator.h"
#include "infrastructure/actuators/ExternalLedActuator.h"
#include "application/events/EventNotifier.h"
#include "presentation/observers/BuzzerObserver.h"
#include "presentation/observers/LedObserver.h"
#include "presentation/observers/SerialObserver.h"
#include "presentation/observers/WebSocketObserver.h"
#include "infrastructure/loaders/OTALoader.h"
#include "domain/SensorData.h"
#include "application/reporting/DailyTemperatureReporter.h"
#include "infrastructure/reporting/EepromSentDateStore.h"
#include "infrastructure/reporting/TelegramBotSender.h"
#include "infrastructure/time/KyivDateTimeSource.h"

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
KyivDateTimeSource kyivDateTime;
EepromSentDateStore sentDateStore;
TelegramBotSender telegramBotSender(TELEGRAM_BOT_TOKEN, TELEGRAM_CHAT_ID);
DailyTemperatureReporter dailyTemperatureReporter(
    sensorData, kyivDateTime, systemClock, telegramBotSender, sentDateStore
);
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
WebServer webServer(webSocket, fileSystem, sensorUpdateService, dailyTemperatureReporter);

ExternalLedActuator externalLedActuator(GREEN_LED_PIN);
BuzzerActuator buzzerActuator(BUZZER_PIN);

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

    if (!fileSystem.begin()) {
        Serial.println("Failed to mount LittleFS");
    }
    if (!dailyTemperatureReporter.begin()) {
        Serial.println("Failed to load Telegram report state; daily reports disabled");
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
        kyivDateTime.begin();
        webServer.begin();
        OTA.begin();
        networkServicesStarted = true;
    }

    if (networkServicesStarted) {
        OTA.handle();
    }

    dailyTemperatureReporter.update(wifiManager.isConnected());
}
