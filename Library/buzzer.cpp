// file:   buzzer.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of BmLib. BmLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: BmLib is distributed without any warranty.


#include "buzzer.h"

Buzzer::Buzzer()
{
    m_pin = -1;
}

Buzzer::~Buzzer()
{
    setOff();
}

void Buzzer::setPin( uint8_t pin)
{
    m_pin = pin;
    pinMode( pin, OUTPUT);
}

void Buzzer::setOn()
{
    if ( m_pin < 0 ) return;
    Actuator::setOn();
    digitalWrite( m_pin, HIGH);
}

void Buzzer::setOff()
{
    if ( m_pin < 0 ) return;
    Actuator::setOff();
    digitalWrite( m_pin, LOW);
}
