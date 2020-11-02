// file:   sensor.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#include "sensor.h"

Sensor::Sensor()
{
    m_dataready = false;
}

Sensor::~Sensor()
{
}

void Sensor::read()
{
    m_dataready = true;
}

bool Sensor::dataReady()
{
    if ( m_dataready ) {
        m_dataready = false;
        return true;
    }
    return false;
}
