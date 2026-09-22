#include "application/WsMessageHandler.h"

WsMessageType WsMessageHandler::handle(const String& message) const {
    if (message == "getValues") {
        return WsMessageType::GET_VALUES;
    }

    return WsMessageType::UNKNOWN;
}
