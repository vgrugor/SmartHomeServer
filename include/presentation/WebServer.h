#ifndef WEB_SERVER_FACADE_H
    #define WEB_SERVER_FACADE_H

    #include <Arduino.h>
    #include <ESPAsyncWebServer.h>
    #include "application/SensorUpdateService.h"
    #include "infrastructure/fs/FileSystem.h"
    #include "infrastructure/web/WebSocket.h"
    #include "config/DeviceConfig.h"

    class WebServer {
        private:
            AsyncWebServer server;
            WebSocket& webSocket;
            FileSystem& fileSystem;
            SensorUpdateService& sensorUpdateService;
            void handleRoot(AsyncWebServerRequest* request);
            void handleHouseTemperature(AsyncWebServerRequest* request);
            void handleOutdoorTemperature(AsyncWebServerRequest* request);
            void handleShowerUpdate(AsyncWebServerRequest* request);
            bool parseFloatParam(AsyncWebServerRequest* request, const String& name, float& value) const;

        public:
            WebServer(
                WebSocket& webSocket,
                FileSystem& fileSystem,
                SensorUpdateService& sensorUpdateService
            );
            void begin();
    };

#endif
