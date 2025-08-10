// env.h
#ifndef ENV_H
    #define ENV_H

    #include "Arduino.h"

    extern const char* OTA_HOSTNAME;
    extern const char* OTA_PASSWORD;

    extern const char* WIFI_SSID;
    extern const char* WIFI_PASSWORD;
    extern const char* WIFI_IP;
    extern const char* WIFI_GATEWAY;
    extern const char* WIFI_SUBNET;

    extern const int GREEN_LED_PIN;
    extern const int BUZZER_PIN;

    extern const int FRONT_LED_MATRIX;
    extern const int MIDDLE_LED_MATRIX;
    extern const int BACK_LED_MATRIX;

    extern const int BOARD_LED;

    extern const unsigned char TEMPERATURE_SENSOR_PIN;
    extern const unsigned long TEMPERATURE_READ_PERIOD;
    extern const unsigned long TEMPERATURE_FIRST_READ_AFTER_LOAD_INTERVAL;
    extern const unsigned long TEMPERATURE_SEND_PERIOD;
    extern const unsigned long WS_CLIENT_UPDATE_PERIOD;

    extern const String TEMPERATURE_HOUSE_PARAM_NAME;
    extern const String TEMPERATURE_OUTDOOR_PARAM_NAME;
    extern const String TEMPERATURE_WATER_SHOWER_PARAM_NAME;
    extern const String VOLUME_WATER_SHOWER_PARAM_NAME;
    extern const String BATTERY_VOLTAGE_SHOWER_PARAM_NAME;
    extern const String BATTERY_PERCENT_SHOWER_PARAM_NAME;

    extern const int SCHEDULER_MAX_TASKS_COUNT;

#endif // ENV_H
