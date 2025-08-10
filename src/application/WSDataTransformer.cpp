#include "application/WSDataTransformer.h"

WsDataTransformer::WsDataTransformer(
    SensorData& sensorData
) : sensorData(sensorData)
{}

String WsDataTransformer::toJSON()
{
    JSONVar sliderValues;

    //sliderValues["sliderValue1"] = String(this->getAllLedMatrixLevel());
    //sliderValues["sliderValue2"] = String(this->getTimerMinute());
    //sliderValues["sliderValue3"] = String(this->getFrontLedMatrixLevel());
    //sliderValues["sliderValue4"] = String(this->getMiddleLedMatrixLevel());
    //sliderValues["sliderValue5"] = String(this->getBackLedMatrixLevel());
    //sliderValues["sliderValue6"] = String(this->getTemperature());

    return JSON.stringify(sliderValues);
}
