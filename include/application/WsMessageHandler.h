#ifndef WS_MESSAGE_HANDLER_H
    #define WS_MESSAGE_HANDLER_H

    #include <Arduino.h>

    enum class WsMessageType {
        GET_VALUES,
        UNKNOWN,
    };

    class WsMessageHandler
    {
        public:
            WsMessageType handle(const String& message) const;
    };

#endif
