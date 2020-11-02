// file:   sensor.h
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#ifndef SENSOR_H
#define SENSOR_H

#include "device.h"

class Sensor : public Device
{
public:
    Sensor();
    ~Sensor();

    virtual void read();
    bool dataReady();

private:

    bool m_dataready;
};

#endif // SENSOR_H
