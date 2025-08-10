#ifndef WS_MESSAGE_HANDLER_H
    #define WS_MESSAGE_HANDLER_H

    #include <Arduino.h>

    class WsMessageHandler
    {
        public:
            WsMessageHandler();
            bool handle(String message);

        private:
    };

#endif
