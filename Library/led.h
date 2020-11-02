// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#ifndef LED_H
#define LED_H

#include "actuator.h"

#define BRIGHT 100
#define NOBLINK 0

class Led : public Actuator
{
public:
    Led();
    ~Led();

    void setPin( uint8_t pin, bool pwm = false);
    void setInversion( bool invert = true);

    void setBrightness( uint8_t on, uint8_t off = 0); // percentage
    void setBlink( uint32_t on = NOBLINK, uint32_t off = NOBLINK ); // millisec
    void setOn();
    void setOff();

private:
    int8_t   m_pin;
    bool     m_pwm;
    uint16_t m_max;
    uint16_t m_min;
    uint32_t m_ton;
    uint32_t m_toff;
    bool     m_isblinking;
    bool     m_invert;
};

#endif // LED_H
