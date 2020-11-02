// file:   switch.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#include "switch.h"

Switch::Switch()
{
    m_pin = -1;
    m_conn = VCC;
    m_pressed = false;
    m_changed = false;
    m_normalopen = true;
}

Switch::~Switch()
{
}

void Switch::setPin( uint8_t pin, uint8_t connect)
{
    m_pin = pin;
    m_conn = connect;
    if ( m_conn == GND )
        pinMode( pin, INPUT_PULLUP);
    else
#ifdef RPI
        pinMode( pin, INPUT_PULLDOWN);
#else
        pinMode( pin, INPUT);
#endif
}

void Switch::setNormalOpen( bool normalopen)
{
    m_normalopen = normalopen;
}

void Switch::triggerEvent( void (*callback)())
{
    // note that switches trigger both events a few times
#ifdef RPI
    attachInterrupt( m_pin, callback, CHANGE);
#else
    attachInterrupt( digitalPinToInterrupt( m_pin), callback, CHANGE);
#endif
}

void Switch::read()
{
    bool press;
    if ( m_conn == VCC )
        press = (digitalRead( m_pin) ? true : false); // internal pullup
    else
        press = (digitalRead( m_pin) ? false : true); // internal pulldown
    if ( !m_normalopen )
        press = !press;
    m_changed = (press != m_pressed);
    m_pressed = press;
    Sensor::read();
}

bool Switch::pressed()
{
    return m_pressed;
}

bool Switch::released()
{
    return !m_pressed;
}

bool Switch::changed()
{
    return m_changed;
}
