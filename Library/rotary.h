// file:   rotary.h
// Copyright 2020 D.E.Repolev
//
// This file is part of BmLib. BmLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: BmLib is distributed without any warranty.


#ifndef ROTARY_H
#define ROTARY_H

#include "sensor.h"

class Rotary : public Sensor
{
public:

    enum Motion { Stop, Forward, Backward };

    Rotary();
    ~Rotary();

    void setPin( uint8_t pin1, uint8_t pin2);
    void setTicksPerRotation( uint16_t ticks);
    void swapDirection();
    void setTimeout( uint32_t msec); // timeout before Motion becomes Stop 

    void read();

    float rpm();  // rotations per minute
    float rps();  // rotations per second

    bool moving();
    bool forward();
    bool backward();

private:

    int8_t   m_pin1;
    int8_t   m_pin2;
    bool     m_prv1;
    bool     m_prv2;
    bool     m_cur1;
    bool     m_cur2;
    bool	 m_swap;
    Motion	 m_move;
    uint16_t m_ticks;
    uint32_t m_tm;
    uint32_t m_tmo;
    float    m_rps;
};

#endif // ROTARY_H
