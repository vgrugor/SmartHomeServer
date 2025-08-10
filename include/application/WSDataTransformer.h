#ifndef WS_DATA_TRANSFORMER_H
    #define WS_DATA_TRANSFORMER_H

    #include <Arduino.h>
    #include <map>
    #include <Arduino_JSON.h>
    #include "domain/SensorData.h"

    class WsDataTransformer {
        public:
            WsDataTransformer(SensorData& sensorData);
            String toJSON();

        private:
            SensorData& sensorData;
    };

#endif
