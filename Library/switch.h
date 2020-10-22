// file:   switch.h
// Copyright 2020 D.E.Repolev
//
// This file is part of BmLib. BmLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: BmLib is distributed without any warranty.


#ifndef SWITCH_H
#define SWITCH_H

#include "sensor.h"

#define GND 0
#define VCC 1

class Switch : public Sensor
{
public:

    Switch();
    ~Switch();

    void setPin( uint8_t pin, uint8_t connect);
    void setNormalOpen( bool normalopen);
    void triggerEvent( void (*callback)());

    void read();

    bool pressed();
    bool released();
    bool changed();

private:

    int8_t	m_pin;
    uint8_t m_conn;
    bool	m_normalopen;
    bool	m_pressed;
    bool    m_changed;
};

#endif // SWITCH_H
