#include "application/WsMessageHandler.h"

WsMessageHandler::WsMessageHandler() 
{}

bool WsMessageHandler::handle(String message) {
    bool result = false;

    String messageValue = message.substring(2);
    int value = messageValue.toInt();

    if (message.indexOf("1s") >= 0) {
        //this->lightManagerService.changeAllLedMatrixLevel(value);

        result = true;
    }

    if (message.indexOf("2s") >= 0) {
        //this->lightManagerService.changeTimerMinute(value);

        result = true;
    }

    if (message.indexOf("3s") >= 0) {
        //this->lightManagerService.changeFrontLedMatrixLevel(value);

        result = true;
    }

    if (message.indexOf("4s") >= 0) {
        //this->lightManagerService.changeMiddleLedMatrixLevel(value);

        result = true;
    }

    if (message.indexOf("5s") >= 0) {
        //this->lightManagerService.changeBackLedMatrixLevel(value);

        result = true;
    }

    if (message.indexOf("getValues") >= 0) {

        result = true;
    }

    return result;
}
