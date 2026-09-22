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
            this->handleMessage(client, arg, data, len);
            break;
        case WS_EVT_PONG:
        case WS_EVT_PING:
        case WS_EVT_ERROR:
            break;
    }
}

void WebSocket::handleMessage(AsyncWebSocketClient* client, void* arg, const uint8_t* data, size_t len) {
    AwsFrameInfo* info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
        String message;
        message.reserve(len);

        for (size_t index = 0; index < len; index++) {
            message += static_cast<char>(data[index]);
        }

        if (this->wsMessageHandler.handle(message) == WsMessageType::GET_VALUES) {
            this->notifyClient(client);
        }
    }
}

void WebSocket::notifyClient(AsyncWebSocketClient* client) {
    client->text(this->wsDataTransformer.toJSON());
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
