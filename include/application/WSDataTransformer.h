#ifndef WS_DATA_TRANSFORMER_H
#define WS_DATA_TRANSFORMER_H

#include <string>
#include "application/network/MonotonicClock.h"
#include "domain/SensorData.h"

class WsDataTransformer {
    public:
        WsDataTransformer(
            const SensorData& sensorData,
            const MonotonicClock& clock
        );
        std::string toJSON() const;

    private:
        const SensorData& sensorData;
        const MonotonicClock& clock;
};

#endif // WS_DATA_TRANSFORMER_H
