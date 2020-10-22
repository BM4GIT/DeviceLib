// file:   buzzer.h
// Copyright 2020 D.E.Repolev
//
// This file is part of BmLib. BmLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: BmLib is distributed without any warranty.


#ifndef BUZZER_H
#define BUZZER_H

#include "actuator.h"

class Buzzer : public Actuator
{
public:
    Buzzer();
    ~Buzzer();

    void setPin( uint8_t pin);

    void setOn();
    void setOff();

protected:
    int8_t m_pin;
};

#endif // BUZZER_H
