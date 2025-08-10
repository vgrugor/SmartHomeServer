#ifndef SENSOR_DATA_H
    #define SENSOR_DATA_H

    #include <unordered_map>
    #include <string>
    #include "SensorType.h"
    #include "presentation/EventNotifier.h"
    #include "domain/SensorType.h"

    class SensorData {
        private:
            std::unordered_map<SensorType, float> data;
            void  pushEvent(const SensorType& key, float value);

        public:
            void setValue(const SensorType& key, float value);
            float getValue(const SensorType& key) const;
            bool hasKey(const SensorType& key) const;
            void removeKey(const SensorType& key);
            void clear();
    };

#endif
