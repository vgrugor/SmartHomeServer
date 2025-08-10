#ifndef EVENT_TYPE_H
    #define EVENT_TYPE_H

    enum class EventType {
        //WiFi events
        WIFI_START_CONNECT,
        WIFI_TRY_CONNECT,
        WIFI_CONNECTED,
        WIFI_RECONNECT,
        
        //temperature events
        READ_TEMPERATURE,
        SEND_TEMPERATURE,

        //web socket events
        WEB_SOCKET_NOTIFY_CLIENT,

        //timer events
        TIMER_SET,
        TIMER_APPLIED,
    };

#endif
