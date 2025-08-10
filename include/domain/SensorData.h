#ifndef SENSOR_DATA_H
    #define SENSOR_DATA_H

    #include <unordered_map>
    #include <string>

    class SensorData {
        private:
            std::unordered_map<std::string, float> data;

        public:
            void setValue(const std::string& key, float value);
            float getValue(const std::string& key) const;
            bool hasKey(const std::string& key) const;
            void removeKey(const std::string& key);
            void clear();
    };

#endif
