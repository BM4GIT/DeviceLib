// file:   distance.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#include "hcsr04.h"

namespace HCSR04
{

/////////////
// AVERAGE //
/////////////

Average::Average()
{
    clear();
}

void Average::clear()
{
    m_cur = 0;
    m_complete = false;
}

void Average::add( float value)
{
    m_val[m_cur] = value;
    m_cur++;
    if ( m_cur == 20 ) {
        m_cur = 0;
        m_complete = true;
    }
}

float Average::avg()
{
    float sum = 0;
    int x = (m_complete ? 20 : m_cur + 1);
    for ( int i = 0; i < x; i++ )
        sum += m_val[i];
    return sum / x + 0.5;
}


//////////////
// DISTANCE //
//////////////

Distance::Distance()
{
    m_trigger = 0;
    m_echo = 0;
}

Distance::~Distance()
{
}

void Distance::setPin( uint8_t trigger, uint8_t echo)
{
    m_trigger = trigger;
    m_echo = echo;
    pinMode( m_trigger, OUTPUT);
    pinMode( m_echo, INPUT);
}

void Distance::read()
{
    digitalWrite( m_trigger, HIGH);
    delayMicroseconds( 10);
    digitalWrite( m_trigger, LOW);

    uint32_t now = micros();
    uint32_t timeout = 500000;

    while ( (digitalRead( m_echo) == LOW) && (micros() - now < timeout) );

    uint32_t startTime = micros();
    while ( (digitalRead( m_echo) == HIGH) && (micros() - now < timeout) );
    uint32_t endTime = micros();

    m_cm = (endTime - startTime) / 58.138;
    Sensor::read();
}


uint16_t Distance::cm()
{
    return m_cm;
}

uint16_t Distance::inch()
{
    return m_cm / 2.54;
}

} //end namespace
