// file:   distance.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#include "hcsr501.h"

namespace HCSR501
{

Motion::Motion()
{
    m_pin = -1;
}

Motion::~Motion()
{
}

void Motion::setPin( uint8_t pin)
{
    m_pin = pin;
    pinMode( m_pin, INPUT);
}

void Motion::read()
{
    m_detect = (digitalRead( m_pin) == HIGH);
    Sensor::read();
}


bool Motion::detected()
{
    return m_detect;
}

} // end namespace
