#ifndef WEBSOCKET_H
    #define WEBSOCKET_H

    #include <Arduino.h>
    #include <ESPAsyncWebServer.h>
    #include <Arduino_JSON.h>
    #include "application/WsMessageHandler.h"
    #include "application/WsDataTransformer.h"

    class WebSocket {
    private:
        WsMessageHandler& wsMessageHandler;
        WsDataTransformer& wsDataTransformer;
        AsyncWebSocket webSocket;
        void handleEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len);
        void handleMessage(void* arg, uint8_t* data, size_t len);

    public:
        WebSocket(WsMessageHandler& wsMessageHandler, WsDataTransformer& wsDataTransformer);
        AsyncWebSocket* getWebSocketObject();
        void notifyClients();
        void cleanupClients();
    };

#endif // WEBSOCKET_H
