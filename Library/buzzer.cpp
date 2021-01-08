// file:   buzzer.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#include "buzzer.h"

Buzzer::Buzzer()
{
    m_pin = -1;
}

Buzzer::~Buzzer()
{
    off();
}

void Buzzer::setPin( uint8_t pin)
{
    m_pin = pin;
    pinMode( pin, OUTPUT);
}

void Buzzer::on()
{
    if ( m_pin < 0 ) return;
    Actuator::on();
    digitalWrite( m_pin, HIGH);
}

void Buzzer::off()
{
    if ( m_pin < 0 ) return;
    Actuator::off();
    digitalWrite( m_pin, LOW);
}
