// file:	hx711.h
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#ifndef HX711_H_
#define HX711_H_

#include "sensor.h"

namespace HX711 {

class Scale : public Sensor
{
public:

    Scale();
    ~Scale();

    void setPin( uint8_t pinClk, uint8_t pinOut);

    void read();

    uint16_t kg();

private:

    long getValue();
    long averageValue( uint8_t times = 25);
    void calibrate( long rawvalue);
    void setScale( float scale = 1992.f);

    int8_t m_clk;
    int8_t m_out;
    int8_t m_kg;

    long   m_value;
    long   m_zero;
    float  m_scale;
};

} // end namespace

#endif /* HX711_H_ */
