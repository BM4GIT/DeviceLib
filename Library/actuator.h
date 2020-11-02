// file:   actuator.h
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#ifndef ACTUATOR_H
#define ACTUATOR_H

#include "device.h"

class Actuator : public Device
{
public:
    Actuator();
    ~Actuator();

    virtual void setOn();
    virtual void setOff();
    bool isOn();

protected:
    bool m_ison;
};

#endif // ACTUATOR_H
