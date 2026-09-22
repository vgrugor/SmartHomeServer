#ifndef WEB_SERVER_FACADE_H
    #define WEB_SERVER_FACADE_H

    #include <Arduino.h>
    #include <ESPAsyncWebServer.h>
    #include "application/events/EventNotifier.h"
    #include "application/SensorValueValidator.h"
    #include "infrastructure/fs/FileSystem.h"
    #include "infrastructure/web/WebSocket.h"
    #include "domain/SensorData.h"
    #include "config/DeviceConfig.h"
    #include "domain/SensorType.h"

    class WebServer {
        private:
            AsyncWebServer server;
            WebSocket& webSocket;
            FileSystem& fileSystem;
            SensorData& sensorData;
            SensorValueValidator sensorValueValidator;
            void handleRoot(AsyncWebServerRequest* request);
            void handleHouseTemperature(AsyncWebServerRequest* request);
            void handleOutdoorTemperature(AsyncWebServerRequest* request);
            void handleShowerUpdate(AsyncWebServerRequest* request);
            bool parseFloatParam(AsyncWebServerRequest* request, const String& name, float& value) const;
            void saveSensorValue(SensorType type, float value);
            void notifyClients();

        public:
            WebServer(
                WebSocket& webSocket,
                FileSystem& fileSystem,
                SensorData& sensorData
            );
            void begin();
    };

#endif
