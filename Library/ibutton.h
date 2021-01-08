// file:   ibutton.h
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#ifndef IBUTTON_H
#define IBUTTON_H

#include "sensor.h"

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

    bool crc8( uint8_t* data, uint8_t len);
    uint8_t readOneByte( uint8_t pin);
    uint8_t readOneBit( uint8_t pin);
    void writeOneByte( uint8_t pin, int byte);
    void writeOneBit( uint8_t pin, int b);
    int isPresent( uint8_t pin);

private:

    uint32_t m_timeout;
    uint32_t m_stamp;
    String   m_id;
    uint8_t  m_pin;
};

#endif // IBUTTON_H
