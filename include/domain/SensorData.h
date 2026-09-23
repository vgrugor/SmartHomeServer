#ifndef SENSOR_DATA_H
    #define SENSOR_DATA_H

    #include <cstdint>
    #include <unordered_map>
    #include "domain/SensorType.h"

    class SensorData {
        private:
            struct Reading {
                float value;
                uint32_t updatedAtMs;
            };

            std::unordered_map<SensorType, Reading> data;
        public:
            void setValue(const SensorType& key, float value, uint32_t updatedAtMs = 0);
            float getValue(const SensorType& key) const;
            uint32_t getUpdatedAtMs(const SensorType& key) const;
            bool hasKey(const SensorType& key) const;
            void removeKey(const SensorType& key);
            void clear();
    };

#endif
