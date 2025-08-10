#ifndef WEB_SERVER_FACADE_H
    #define WEB_SERVER_FACADE_H

    #include <Arduino.h>
    #include <ESPAsyncWebServer.h>
    #include "infrastructure/web/WebSocket.h"
    #include "infrastructure/fs/FileSystem.h"
    #include "domain/SensorData.h"
    #include "infrastructure/env.h"
    #include "domain/SensorType.h"

    class WebServer {
        private:
            AsyncWebServer server;
            WebSocket& webSocket;
            FileSystem& fileSystem;
            SensorData& sensorData;
            void handleRoot(AsyncWebServerRequest* request);
            void handleHouseTemperature(AsyncWebServerRequest* request);
            void handleOutdoorTemperature(AsyncWebServerRequest* request);
            void handleShowerUpdate(AsyncWebServerRequest* request);

        public:
            WebServer(
                WebSocket& webSocket, 
                FileSystem& fileSystem, 
                SensorData& sensorData
            );
            void begin();
            void handleClient();
    };

#endif
