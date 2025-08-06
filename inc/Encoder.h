#ifndef ENCODER_LIB_H
#define ENCODER_LIB_H

#include "foc_utils.h"
#include "Arduino_wrapper.h"
#include "Sensor.h"

class Encoder: public Sensor{
    public: 
        Encoder(int ch, float ppr);
        int channel;

        void init() override;

        float cpr;//!< encoder cpr number

        // Abstract functions of the Sensor class implementation
        /** get current angle (rad) */
        float getSensorAngle() override;
        virtual void update() override;

    private:
        volatile long pulse_counter;
        volatile long pulse_timestamp;

        // velocity calculation variables
        float prev_Th, pulse_per_second;
        volatile long prev_pulse_counter, prev_timestamp_us;
};
#endif