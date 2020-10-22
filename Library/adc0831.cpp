// file:   adc0831.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of BmLib. BmLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: BmLib is distributed without any warranty.


#include "adc0831.h"

namespace ADC0831
{

ADConverter::ADConverter()
{
    m_pinCS = -1;
    m_pinCLK = -1;
    m_pinDO = -1;
    m_raw = 0;
    m_minRaw = 0;
    m_maxRaw = 255;
    m_minVal = 0;
    m_maxVal = 100;
}

ADConverter::~ADConverter()
{
}

void ADConverter::setPin( uint8_t pinCS, uint8_t pinCLK, uint8_t pinDO)
{
    m_pinCS = pinCS;
    m_pinCLK = pinCLK;
    m_pinDO = pinDO;
    pinMode( m_pinCS, OUTPUT);
    pinMode( m_pinCLK, OUTPUT);
    pinMode( m_pinDO, INPUT);
    digitalWrite( m_pinCS, HIGH);
    digitalWrite( m_pinCLK, LOW);
}

void ADConverter::setInputRange( uint8_t min, uint8_t max)
{
    m_maxRaw = max;
    m_minRaw = min;
}

void ADConverter::setOutputRange( float min, float max)
{
    m_maxVal = max;
    m_minVal = min;
}

void ADConverter::read()
{
    m_raw = 0;
    digitalWrite( m_pinCS, LOW); 
    digitalWrite( m_pinCLK, HIGH);
    delayMicroseconds( 2);
    digitalWrite( m_pinCLK, LOW);
    delayMicroseconds( 2);

    for ( int i = 7; i >= 0; i-- )
    {
        digitalWrite( m_pinCLK, HIGH);
        delayMicroseconds( 2);
        digitalWrite( m_pinCLK, LOW);
        delayMicroseconds( 2);
        if ( digitalRead( m_pinDO) )
          m_raw |= (1 << i);
    }

    digitalWrite( m_pinCS, HIGH);
}

float ADConverter::value()
{
    float val = m_minVal + (m_raw - m_minRaw) * (m_maxVal - m_minVal) / (m_maxRaw - m_minRaw);
    return val;
}

uint8_t ADConverter::rawValue()
{
    return m_raw;
}

} // end namespace
