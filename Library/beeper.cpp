// file:   beeper.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#include "beeper.h"

Beeper::Beeper()
{
    m_pin = -1;
}

Beeper::~Beeper()
{
    off();
}

void Beeper::setPin( uint8_t pin)
{
    m_pin = pin;
    pinMode( pin, OUTPUT);
}

void Beeper::setBeeper( float herz, uint16_t msec)
{
    m_beep = herz;
    m_dur = msec;
}

void Beeper::beep()
{
    if ( m_pin < 0 ) return;
    Actuator::on();
    playtone( m_beep, m_dur);
    Actuator::off();
}

uint8_t Beeper::addTone( float herz, uint16_t msec)
{
    uint8_t ix = m_hz.add( herz);
    m_tm.add( msec);
    return ix;
}

void Beeper::insertTone( uint8_t pos, float herz, uint16_t msec)
{
    m_hz.insert( pos, herz);
    m_tm.insert( pos, msec);
}

void Beeper::remove( uint8_t pos, uint8_t count)
{
    for ( int i = pos; i < pos + count; i++ ) {
        m_hz.removeAt( i);
        m_tm.removeAt( i);
    }
}

void Beeper::clear()
{
    m_hz.removeAll();
    m_tm.removeAll();
}

void Beeper::play()
{
    if ( m_pin < 0 ) return;
    Actuator::on();
    for (int i = 0; i < m_hz.size(); i++ ) {
        playtone( m_hz[i], m_tm[i]);
        if ( !Actuator::isOn() ) break;
    }
    Actuator::off();
}

void Beeper::on()
{
    if ( m_pin < 0 ) return;
    Actuator::on();
    playtone( m_beep, 0); // play continuously until off
}

void Beeper::off()
{
    if ( m_pin < 0 ) return;
    Actuator::off();
}

void Beeper::playtone( float herz, uint16_t msec)
{
    uint32_t t = (float) 500000 / herz;
    uint32_t tm = millis() + msec;
    while ( !msec || ((uint32_t) millis() < tm) ) {
        digitalWrite( m_pin, HIGH);
        delayMicroseconds( t);
        digitalWrite( m_pin, LOW);
        delayMicroseconds( t);
        if ( !Actuator::isOn() ) break;
    }
}
