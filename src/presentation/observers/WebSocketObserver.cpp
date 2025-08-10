#include "presentation/observers/WebSocketObserver.h"

WebSocketObserver::WebSocketObserver(WebSocket& webSocket) : webSocket(webSocket) {}

void WebSocketObserver::update(EventType eventType, const String& message) {
    switch (eventType) {
        //web socket events
        case EventType::WEB_SOCKET_NOTIFY_CLIENT:
            this->webSocket.notifyClients();

            break;
    }
}
