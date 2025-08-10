#include "infrastructure/web/WebSocket.h"

WebSocket::WebSocket(
    WsMessageHandler& wsMessageHandler, 
    WsDataTransformer& wsDataTransformer
) : wsMessageHandler(wsMessageHandler), 
    wsDataTransformer(wsDataTransformer), 
    webSocket("/ws") 
{
    webSocket.onEvent([this](AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
        this->handleEvent(server, client, type, arg, data, len);
    });
}

void WebSocket::handleEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());

            if (webSocket.count() > 5) {
                this->cleanupClients();
            }

            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
            Serial.println("WebSocket client event WS_EVT_DATA");
            this->handleMessage(arg, data, len);
            break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}

void WebSocket::handleMessage(void* arg, uint8_t* data, size_t len) {
    AwsFrameInfo* info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        data[len] = 0;
        String message = (char*)data;

        if (this->wsMessageHandler.handle(message)) {
            this->notifyClients();
        }
    }
}

void WebSocket::notifyClients() {
    this->webSocket.textAll(this->wsDataTransformer.toJSON());
}

AsyncWebSocket* WebSocket::getWebSocketObject() {
    return &webSocket;
}

void WebSocket::cleanupClients() {
    this->webSocket.cleanupClients();
}
