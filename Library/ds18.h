// file:   dst.h
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#ifndef DS18_H
#define DS18_H

// Max sample rate Dallas w1 sensor is 1.3 Hz (duty cycle 750 ms)

#include "sensor.h"
#include "linkedlist.h"
//#ifdef RPI
//#include <vector>
//#endif

namespace DS18
{

typedef LinkedList<String> StringList;

class Temperature : public Sensor
{
public:

	Temperature();
	~Temperature() {};

    static StringList getIdList( uint8_t pin = 10);
    void setSensor( String id, uint8_t pin = 10);

    void read();

    float kelvin();
    float celcius();
    float fahrenheit();

private:

#ifdef RPI
    String  m_id;
#else
    int     m_id;
#endif
    int8_t m_pin;
    float   m_temp;
};

} // end namespace

#endif
