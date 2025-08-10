#ifndef WEB_SOCKED_OBSERVER_H
    #define WEB_SOCKED_OBSERVER_H

    #include <Arduino.h>
    #include "Observer.h"
    #include "presentation/EventType.h"
    #include "infrastructure/web/WebSocket.h"

    class WebSocketObserver : public Observer {
        private:
            WebSocket& webSocket;

        public:
            WebSocketObserver(WebSocket& webSocket);
            void update(EventType eventType, const String& message = "") override;
    };

#endif // WEB_SOCKED_OBSERVER_H
