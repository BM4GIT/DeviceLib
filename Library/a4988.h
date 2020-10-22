// file:   stepper.h
// Copyright 2020 D.E.Repolev
//
// This file is part of BmLib. BmLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: BmLib is distributed without any warranty.


#ifndef A4988_H
#define A4988_H

#include "actuator.h"

namespace A4988
{

#define TURN    0
#define MOVE    1
#define ROTATE  2
#define GO      3

class Stepper : public Actuator
{
public:

    Stepper();
    ~Stepper();

    void init( uint16_t stepsPerRotation = 200);

    void setPin( uint8_t pinStep,
                 uint8_t pinDir,
                 uint8_t pinEnab,
                 uint8_t pinReset);

    void setForward( bool forward = true);
    void setReverse();
    void setHold( bool hold = true);
    void setRelease();

    void turn( uint32_t steps, uint8_t speed = 100);
    void rotate( uint32_t degr, uint8_t speed = 100);
    void move( uint32_t msec, uint8_t = 100);
    void go( uint8_t speed = 100);
    void setOff();

#ifdef RPI
    void start( uint8_t command, uint32_t param, uint8_t speed = 100);
#endif
    bool isOn();

private:

    void stepBiPolar();
    void off();

    int8_t   m_pin1;
    int8_t   m_pin2;
    int8_t   m_pin3;
    int8_t   m_pin4;
    uint16_t m_stepspr; // steps per rotation
    bool     m_forward; // true = forward, false = reverse
    bool     m_hold;    // true = hold torque,
    uint32_t m_step;
#ifdef RPI
    thread  *m_run;
#endif
};

} // end namespace

#endif // A4988_H
