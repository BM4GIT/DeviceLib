// file:   stepper.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#include "unistepper.h"

namespace UniStepper
{

int g_coils[8] = { 0b01000, 0b01100, 0b00100, 0b00110, 0b00010, 0b00011, 0b00001, 0b01001};

#ifdef RPI
void stepperWorker( Stepper *st, uint8_t command, uint32_t param, uint8_t speed)
{
    switch ( command ) {
        case TURN :   st->turn( param, speed); break;
        case MOVE :   st->move( param, speed); break;
        case ROTATE : st->rotate( param, speed); break;
        case GO :     st->go( speed); break;
        default :     st->setOff();
    }
}
#endif

Stepper::Stepper() : Actuator()
{
    m_pin1 = -1;
    m_pin2 = -1;
    m_pin3 = -1;
    m_pin4 = -1;
    m_stepspr = 4096; // 1.8 degree stepper
    m_forward = true;
    m_step = 0;
#ifdef RPI
    m_run = nullptr;
#endif
}

Stepper::~Stepper()
{
    setOff();
}

void Stepper::init( uint16_t stepsPerRotation)
{
    m_stepspr = stepsPerRotation;
}

void Stepper::stepUniPolar()
{
    if ( m_forward ) {
        m_step++;
        if ( m_step > 7 ) m_step = 0;
    }
    else {
        m_step--;
        if ( m_step < 0 ) m_step = 7;
    }
    digitalWrite( m_pin1, g_coils[m_step] & 1);
    digitalWrite( m_pin2, g_coils[m_step] & 2);
    digitalWrite( m_pin3, g_coils[m_step] & 4);
    digitalWrite( m_pin4, g_coils[m_step] & 8);
}

void Stepper::off()
{
    if ( !m_hold ) {
        // wait 100 usec to set motor in position
        delayMicroseconds( 100);
        // then release torque
        digitalWrite( m_pin1, 0);
        digitalWrite( m_pin2, 0);
        digitalWrite( m_pin3, 0);
        digitalWrite( m_pin4, 0);
    }
}

void Stepper::setPin( uint8_t pin1,
                      uint8_t pin2,
                      uint8_t pin3,
                      uint8_t pin4)
{
    m_pin1 = pin1;
    m_pin2 = pin2;
    m_pin3 = pin3;
    m_pin4 = pin4;

    pinMode( m_pin1, OUTPUT);
    pinMode( m_pin2, OUTPUT);
    pinMode( m_pin3, OUTPUT);
    pinMode( m_pin4, OUTPUT);
}

void Stepper::setForward( bool forward)
{
    m_forward = forward;
}

void Stepper::setReverse()
{
    m_forward = false;
}

void Stepper::setHold( bool hold)
{
    m_hold = hold;
}

void Stepper::setRelease()
{
    m_hold = false;
}

void Stepper::turn( uint32_t steps, uint8_t speed)
{
    if ( speed > 100 ) return;
    if ( speed )
    {
        Actuator::setOn();

        long dly = (100 - speed) * 100 + 1000;
        while ( steps ) {
            if ( !Actuator::isOn() )
                break;
            stepUniPolar();
            steps--;
            delayMicroseconds( dly);
        }
    }
    Actuator::setOff();
    off();
}

void Stepper::rotate( uint32_t degr, uint8_t speed)
{
    long steps = (long) m_stepspr * degr / 360;
    turn( steps, speed);
}

void Stepper::move( uint32_t msec, uint8_t speed)
{
    if ( speed > 100 ) return;
    if ( speed ) {
        Actuator::setOn();

        long dly = (100 - speed) * 100 + 1000;
        long tm = millis() + msec;
        while ( millis() < tm ) {
            if ( !Actuator::isOn() )
                break;
            stepUniPolar();
            delayMicroseconds( dly);
        }
    }
    Actuator::setOff();
    off();
}

void Stepper::go( uint8_t speed)
{
    if ( speed > 100 ) return;

    if ( speed ) {
        Actuator::setOn();

        long dly = (100 - speed) * 100 + 1000;
        while ( Actuator::isOn() ) {
            stepUniPolar();
            delayMicroseconds( dly);
        }
    }
    Actuator::setOff();
    off();
}

void Stepper::setOff()
{
    Actuator::setOff();
#ifdef RPI
    if ( m_run ) {
        m_run->join();
        delete m_run;
        m_run = nullptr;
    }
#endif
    off();
}

bool Stepper::isOn()
{
    return Actuator::isOn();
}

#ifdef RPI
void Stepper::start( uint8_t command, uint32_t param, uint8_t speed)
{
    m_run = new thread( stepperWorker, this, command, param, speed);
}
#endif

} // end namespace
