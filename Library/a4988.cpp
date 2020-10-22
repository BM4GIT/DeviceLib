// file:   stepper.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of BmLib. BmLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: BmLib is distributed without any warranty.


#include "a4988.h"

namespace A4988
{

#ifdef RPI
void stepperWorker( Stepper *st, uint8_t command, int param, int speed)
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
    m_stepspr = 200; // 1.8 degree stepper
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

void Stepper::stepBiPolar()
{
    digitalWrite( m_pin1, HIGH);
    delayMicroseconds( 10);
    digitalWrite( m_pin1, LOW);
    delayMicroseconds( 10);
}

void Stepper::off()
{
    digitalWrite( m_pin1, 0);
    digitalWrite( m_pin2, 0);
    if ( m_hold )
        digitalWrite( m_pin3, 0);
    else {
        // wait 100 usec to set motor in position
        delayMicroseconds( 100);
        // then release torque
        digitalWrite( m_pin3, 1);
    }
}

void Stepper::setPin( uint8_t pinStep,
                      uint8_t pinDir,
                      uint8_t pinEnab,
                      uint8_t pinReset)
{
    m_pin1 = pinStep;
    m_pin2 = pinDir;
    m_pin3 = pinEnab;
    m_pin4 = pinReset;

    pinMode( m_pin1, OUTPUT);
    pinMode( m_pin2, OUTPUT);
    pinMode( m_pin3, OUTPUT);
    pinMode( m_pin4, OUTPUT);

    digitalWrite( m_pin3, HIGH);
    digitalWrite( m_pin4, HIGH);
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

        digitalWrite( m_pin3, LOW);
        digitalWrite( m_pin2, m_forward ? HIGH : LOW);

        long dly = (100 - speed) * 100 + 1000;
        while ( steps ) {
            if ( !Actuator::isOn() )
                break;
            stepBiPolar();
            steps--;
            delayMicroseconds( dly);
        }

        digitalWrite( m_pin3, HIGH);
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

        digitalWrite( m_pin3, LOW);
        digitalWrite( m_pin2, m_forward ? HIGH : LOW);

        long dly = (100 - speed) * 100 + 1000;
        long tm = millis() + msec;
        while ( millis() < tm ) {
            if ( !Actuator::isOn() )
                break;
            stepBiPolar();
            delayMicroseconds( dly);
        }

        digitalWrite( m_pin3, HIGH);
    }
    Actuator::setOff();
    off();
}

void Stepper::go( uint8_t speed)
{
    if ( speed > 100 ) return;

    if ( speed ) {
        Actuator::setOn();

        digitalWrite( m_pin3, LOW);
        digitalWrite( m_pin2, m_forward ? HIGH : LOW);

        long dly = (100 - speed) * 100 + 1000;
        while ( Actuator::isOn() ) {
            stepBiPolar();
            delayMicroseconds( dly);
        }

        digitalWrite( m_pin3, HIGH);
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
    digitalWrite( m_pin3, HIGH);
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
