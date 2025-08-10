#include "application/WSDataTransformer.h"

WsDataTransformer::WsDataTransformer(
    SensorData& sensorData
) : sensorData(sensorData)
{}

String WsDataTransformer::toJSON()
{
    JSONVar sliderValues;

    sliderValues["sliderValue1"] = String(this->sensorData.getValue(SensorType::HOUSE_TEMP));
    sliderValues["sliderValue2"] = String(this->sensorData.getValue(SensorType::OUTDOOR_TEMP));
    sliderValues["sliderValue3"] = String(this->sensorData.getValue(SensorType::WATER_TEMP));
    sliderValues["sliderValue4"] = String(this->sensorData.getValue(SensorType::WATER_LEVEL_LITER));
    sliderValues["sliderValue5"] = String(this->sensorData.getValue(SensorType::BATTERY_VOLTAGE));
    sliderValues["sliderValue6"] = String(this->sensorData.getValue(SensorType::BATTERY_PERCENT));

    return JSON.stringify(sliderValues);
}
