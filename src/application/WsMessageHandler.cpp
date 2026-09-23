#include "application/WsMessageHandler.h"
#include <cstring>

WsMessageType WsMessageHandler::handle(const char* message, size_t length) const {
    static const char GET_VALUES_COMMAND[] = "getValues";
    static const size_t GET_VALUES_LENGTH = sizeof(GET_VALUES_COMMAND) - 1;

    if (
        message != nullptr
        && length <= MAX_MESSAGE_LENGTH
        && length == GET_VALUES_LENGTH
        && std::memcmp(message, GET_VALUES_COMMAND, GET_VALUES_LENGTH) == 0
    ) {
        return WsMessageType::GET_VALUES;
    }

    return WsMessageType::UNKNOWN;
}
