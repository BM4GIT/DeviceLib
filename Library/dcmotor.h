// file:   dcmotor.h
// Copyright 2020 D.E.Repolev
//
// This file is part of BmLib. BmLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: BmLib is distributed without any warranty.


#ifndef DCMOTOR_H
#define DCMOTOR_H

#include "actuator.h"

#define FULLSPEED 100

class DcMotor : public Actuator
{
public:
    DcMotor();
    ~DcMotor();

    void setPin( uint8_t pin, bool pwm = true);

    void setSpeed( uint8_t speed);
    void setOn();
    void setOff();

protected:
    int8_t   m_pin;
    uint8_t  m_speed;
    bool     m_pwm;
};

#endif // DCMOTOR_H
