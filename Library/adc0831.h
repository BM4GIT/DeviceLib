// file:   adc0831.h
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#ifndef ADC0831_H
#define ADC0831_H

#include "sensor.h"

namespace ADC0831
{
    
class ADConverter : public Sensor
{
public:

    ADConverter();
    ~ADConverter();

    void setPin( uint8_t pinCS, uint8_t pinCLK, uint8_t pinDO);

    void setInputRange( uint8_t min, uint8_t max);
    void setOutputRange( float min, float max);

    void read();

    float value();
    uint8_t rawValue();

private:

    int8_t	m_pinCS;
    int8_t	m_pinCLK;
    int8_t	m_pinDO;
    uint8_t	m_raw;
    uint8_t m_minRaw;
    uint8_t m_maxRaw;
    float   m_minVal;
    float   m_maxVal;
};

} //end namespace

#endif // ADC0831_H
