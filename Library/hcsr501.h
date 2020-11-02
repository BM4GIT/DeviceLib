// file:   hcsr501.h
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#ifndef HCSR501_H
#define HCSR501_H

#include "sensor.h"

namespace HCSR501
{

class Motion : public Sensor
{
public:
    Motion();
    ~Motion();

    void setPin( uint8_t pin);

    void read();

    bool detected();

private:
    int8_t  m_pin;
    bool m_detect;
};

} // end namespace

#endif // HCSR501_H
