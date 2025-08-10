#ifndef EVENT_TYPE_H
    #define EVENT_TYPE_H

    enum class EventType {
        //WiFi events
        WIFI_START_CONNECT,
        WIFI_TRY_CONNECT,
        WIFI_CONNECTED,
        WIFI_RECONNECT,

        //data events
        PARAM_SAVED,

        //web socket events
        WEB_SOCKET_NOTIFY_CLIENT,
    };

#endif
