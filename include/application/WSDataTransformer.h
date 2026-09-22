#ifndef WS_DATA_TRANSFORMER_H
#define WS_DATA_TRANSFORMER_H

#include <string>
#include "domain/SensorData.h"

class WsDataTransformer {
    public:
        explicit WsDataTransformer(const SensorData& sensorData);
        std::string toJSON() const;

    private:
        const SensorData& sensorData;
};

#endif // WS_DATA_TRANSFORMER_H
