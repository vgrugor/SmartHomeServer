#ifndef WEBSOCKET_H
    #define WEBSOCKET_H

    #include <Arduino.h>
    #include <ESPAsyncWebServer.h>
    #include "application/WsMessageHandler.h"
    #include "application/WSDataTransformer.h"

    class WebSocket {
    private:
        WsMessageHandler& wsMessageHandler;
        WsDataTransformer& wsDataTransformer;
        AsyncWebSocket webSocket;
        void handleEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len);
        void handleMessage(AsyncWebSocketClient* client, void* arg, const uint8_t* data, size_t len);
        void notifyClient(AsyncWebSocketClient* client);

    public:
        WebSocket(WsMessageHandler& wsMessageHandler, WsDataTransformer& wsDataTransformer);
        AsyncWebSocket* getWebSocketObject();
        void notifyClients();
        void cleanupClients();
    };

#endif // WEBSOCKET_H
