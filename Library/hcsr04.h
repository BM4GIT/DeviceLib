// file:   distance.h
// Copyright 2020 D.E.Repolev
//
// This file is part of BmLib. BmLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: BmLib is distributed without any warranty.


#ifndef HCSR04_H
#define HCSR04_H

#include "sensor.h"

namespace HCSR04
{

class Average
{
public:
    Average();

    void  add( float value);
    void  clear();
    float avg();

protected:
    float  m_val[20];
    int    m_cur;
    bool   m_complete;
};

class Distance : public Sensor
{
public:
    Distance();
    ~Distance();

    void setPin( uint8_t trigger, uint8_t echo);

    void read();

    uint16_t cm();
    uint16_t inch();

private:
    int8_t     m_trigger;
	int8_t		m_echo;
    int8_t     m_cm;
};

} // end namespace

#endif // DISTANCE_H
