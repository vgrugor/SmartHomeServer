#ifndef WS_MESSAGE_HANDLER_H
#define WS_MESSAGE_HANDLER_H

#include <cstddef>

enum class WsMessageType {
    GET_VALUES,
    UNKNOWN,
};

class WsMessageHandler {
    public:
        static const size_t MAX_MESSAGE_LENGTH = 32;
        WsMessageType handle(const char* message, size_t length) const;
};

#endif // WS_MESSAGE_HANDLER_H
