// file:   rotary.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#include "rotary.h"

Rotary::Rotary()
{
    m_pin1 = -1;
    m_pin2 = -1;
    m_swap = false;
    m_move = Stop;
    m_tmo = 500;
    m_tm = millis();
    m_ticks = 20;
    m_rps = 0;
}

Rotary::~Rotary()
{
}

void Rotary::setPin( uint8_t pin1, uint8_t pin2)
{
    m_pin1 = pin1;
    m_pin2 = pin2;
    pinMode( pin1, INPUT);
    pinMode( pin2, INPUT);
    m_prv1 = m_cur1 = digitalRead( m_pin1);
    m_prv2 = m_cur2 = digitalRead( m_pin2);
}

void Rotary::swapDirection()
{
    m_swap = !m_swap;
}

void Rotary::setTicksPerRotation( uint16_t ticks)
{
    m_ticks = ticks;
}

void Rotary::setTimeout( uint32_t msec)
{
    m_tmo = msec;
    m_tm = millis();
}

void Rotary::read()
{
    m_prv1 = m_cur1;
    m_prv2 = m_cur2;
    m_cur1 = digitalRead( m_pin1);
    m_cur2 = digitalRead( m_pin2);

    if ( m_prv1 == m_cur1 && m_prv2 == m_cur2 ) {
		if ( m_tm + m_tmo < millis() ) {
			m_move = Stop;
            m_rps = 0;
        }
        return;
    }

    m_rps = 1000 / ((float) (millis() - m_tm) * m_ticks);

	//	ROTATION CYCLE PIN1-PIN2:
	//	FORWARD:	T-T > F-T > F-F > T-F > T-T
	//	BACKWARD:	T-T > T-F > F-F > F-T > T-T

	// CHECK THE CHANGE IN ROTARY POSITION					//	possible combinations
    if ( m_prv1 ) {
        if ( m_prv2 ) {
            if ( !m_cur1 && m_cur2 ) m_move = Forward;		//  T-T > F-T
            if ( m_cur1 && !m_cur2 ) m_move = Backward;		//	T-T > T-F
        }
        else {
            if ( m_cur1 && m_cur2 ) m_move = Forward;		//	T-F > T-T
            if ( !m_cur1 && !m_cur2 ) m_move = Backward;	// 	T-F > F-F
        }
    }
    else {
        if ( m_prv2 ) {
            if ( !m_cur1 && !m_cur2 ) m_move = Forward;		//	F-T > F-F
            if ( m_cur1 && m_cur2 ) m_move = Backward;		//	F-T > T-T
        }
        else {
            if ( m_cur1 && !m_cur2 ) m_move = Forward;		//	F-F > T-F
            if ( !m_cur1 && m_cur1 ) m_move = Backward;		//	F-F > F-T
        }
    }

    Sensor::read();
    m_tm = millis();
}

bool Rotary::moving()
{
    return (m_move != Stop);
}

bool Rotary::forward()
{
    return (m_swap ? (m_move == Backward) : (m_move == Forward));
}

bool Rotary::backward()
{
    return (m_swap ? (m_move == Forward) : (m_move == Backward));
}

float Rotary::rpm()
{
    return m_rps * 60;
}

float Rotary::rps()
{
    return m_rps;
}
