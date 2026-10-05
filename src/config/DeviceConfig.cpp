#include "config/DeviceConfig.h"

const int GREEN_LED_PIN = D1;
const int BUZZER_PIN = D0;
const int TFT_CHIP_SELECT_PIN = D8;
const int TFT_DATA_COMMAND_PIN = D2;
const int TFT_RESET_PIN = -1;
const uint32_t TFT_PAGE_DURATION_MS = 5000;

const String TEMPERATURE_HOUSE_PARAM_NAME = "temp";
const String TEMPERATURE_OUTDOOR_PARAM_NAME = "temp";
const String TEMPERATURE_WATER_SHOWER_PARAM_NAME = "temp";
const String VOLUME_WATER_SHOWER_PARAM_NAME = "liter";
const String BATTERY_VOLTAGE_SHOWER_PARAM_NAME = "voltage";
const String BATTERY_PERCENT_SHOWER_PARAM_NAME = "percent";
