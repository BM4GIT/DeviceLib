// file:   ibutton.h
// Copyright 2020 D.E.Repolev
//
// This file is part of BmLib. BmLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: BmLib is distributed without any warranty.


#ifndef IBUTTON_H
#define IBUTTON_H

#include "sensor.h"
#include "ArduinoCore.h"
#include "ArduinoString.h"

extern const String NOBUTTON;
extern const String EXPIRED;
extern const String INVALID;

class IButton : public Sensor
{
public:
    IButton();
    ~IButton();

    void setPin( uint8_t pin);
    void setTimeout( uint32_t timeout); // timeout in millisec

    void read();
    String tag( uint32_t expireAfter = 0); // expired in msec, 0 means 'never'

private:

    uint32_t m_timeout;
    uint32_t m_stamp;
    String   m_id;
};

#endif // IBUTTON_H
