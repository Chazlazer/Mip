#include "Encoder.h"
#include <rc/encoder_eqep.h>

Encoder::Encoder(int ch, float ppr){
    // Encoder Channgel to Read From
    // Cannot be 4
    channel = ch;

    // counter setup
    pulse_counter = 0;
    pulse_timestamp = 0;

    cpr = 4*ppr;

    // velocity calculation variables
    prev_Th = 0;
    pulse_per_second = 0;
    prev_pulse_counter = 0;
    prev_timestamp_us = __micros();   
}

// Sensor update function. Safely copy volatile interrupt variables into Sensor base class state variables.
void Encoder::update() {
    // Copy volatile variables in minimal-duration blocking section to ensure no interrupts are missed
    pulse_counter = rc_encoder_eqep_read(channel);
    // angle_prev_ts = pulse_timestamp;
    angle_prev_ts = __micros();
    long copy_pulse_counter = pulse_counter;
    // TODO: numerical precision issue here if the pulse_counter overflows the angle will be lost
    full_rotations = copy_pulse_counter / (int)cpr;
    angle_prev = _2PI * ((copy_pulse_counter) % ((int)cpr)) / ((float)cpr);
}

/*
	Shaft angle calculation
*/
float Encoder::getSensorAngle(){
    // pulse_counter = quadrature_encoder_get_count(m_pio,m_sm);
    return _2PI * (pulse_counter) / ((float)cpr);
}

void Encoder::init(){
    rc_encoder_eqep_init();

    // counter setup
    pulse_counter = 0;
    pulse_timestamp =__micros();

    // velocity calculation variables
    prev_Th = 0;
    pulse_per_second = 0;
    prev_pulse_counter = 0;
    prev_timestamp_us = __micros();
    
}