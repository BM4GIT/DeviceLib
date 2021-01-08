// file:   dcmotor.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#include "dcmotor.h"

DcMotor::DcMotor()
{
    m_pin = -1;
    m_pwm = true;
}

DcMotor::~DcMotor()
{
    off();
}

void DcMotor::setPin( uint8_t pin, bool pwm)
{
    m_pin = pin;
    m_pwm = pwm;
    m_speed = FULLSPEED;
    pinMode( pin, OUTPUT);
}

void DcMotor::setSpeed( uint8_t speed)
{
    if ( speed < 0 ) speed = 0;
    if ( speed > 100 )
        m_speed = FULLSPEED;
    else
        m_speed = speed * 1023 / 100;
    if ( m_ison )
        on();
}

void DcMotor::on()
{
    if ( m_pin < 0 ) return;
    Actuator::on();
    if ( m_pwm )
        analogWrite( m_pin, m_speed);
    else
        digitalWrite( m_pin, m_speed ? HIGH : LOW);
}

void DcMotor::off()
{
    if ( m_pin < 0 ) return;
    Actuator::off();
    if ( m_pwm )
        analogWrite( m_pin, 0);
    else
        digitalWrite( m_pin, LOW);
}
