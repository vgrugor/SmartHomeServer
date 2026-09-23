#ifndef ACTUATOR_H
    #define ACTUATOR_H

    class Actuator {
        public:
            virtual ~Actuator() = default;
            virtual void begin() = 0;
            virtual void setState(bool state) = 0;
    };

#endif // ACTUATOR_H
