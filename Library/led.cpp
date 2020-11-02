// file:   led.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#include "led.h"
#include "linkedlist.h"

#ifdef RPI
#define PINBRIGHT 1023
#else
#define PINBRIGHT 255
#endif

struct BlinkInfo {
    uint8_t  pin;
    bool     pwm;
    uint16_t max;
    uint16_t min;
    uint32_t ton;
    uint32_t toff;
    uint32_t time;
    bool     blink;
    bool     invert;
};
LinkedList<BlinkInfo*> g_blink;

void onBlinkEvent()
{
    uint32_t tm = millis();
    for ( int i = 0; i < g_blink.size(); i++ ) {
        BlinkInfo *blk = g_blink[i];
        if ( blk->blink && ((uint32_t)(blk->time + blk->ton) <= tm) ) {
            blk->time = tm;
            if ( blk->pwm )
                analogWrite( blk->pin, blk->invert ? PINBRIGHT - blk->min : blk->min);
            else
                digitalWrite( blk->pin, blk->invert ? HIGH : LOW);
            blk->blink = false;
        }
        if ( !blk->blink && ((uint32_t)(blk->time + blk->toff) <= tm) ) {
            blk->time = tm;
            if ( blk->pwm )
                analogWrite( blk->pin, blk->invert ? PINBRIGHT - blk->max : blk->max);
            else
                digitalWrite( blk->pin, blk->invert ? LOW : HIGH);
            blk->blink = true;
        }
    }
}

Led::Led()
{
    m_pin = -1;
    m_pwm = false;
    m_max = PINBRIGHT;
    m_min = 0;
    m_ton = NOBLINK;
    m_toff = NOBLINK;
    m_isblinking = false;
    m_invert = false;
}

Led::~Led()
{
    setOff();
}

void Led::setPin( uint8_t pin, bool pwm)
{
    m_pin = pin;
    m_pwm = pwm;
    pinMode( pin, OUTPUT);
}

void Led::setInversion( bool invert)
{
    m_invert = invert;
}

void Led::setBrightness( uint8_t on, uint8_t off)
{
    if ( on >= BRIGHT )
        m_max = PINBRIGHT;
    else
        m_max = on * PINBRIGHT / BRIGHT;
    if ( off >= BRIGHT )
        m_min = PINBRIGHT;
    else
        m_min = off * PINBRIGHT / BRIGHT;
    if ( m_ison )
        setOn();
}

void Led::setBlink( uint32_t on, uint32_t off)
{
    m_ton = on;
    m_toff = off;
    if ( m_ison )
        setOn();
}

void Led::setOn()
{
    if ( m_pin < 0 ) return;
    Actuator::setOn();
    if ( m_pwm ) {
        analogWrite( m_pin, m_invert ? PINBRIGHT - m_max : m_max);
    }
    else
        digitalWrite( m_pin, m_invert ? (m_max ? LOW : HIGH) : (m_max ? HIGH : LOW));
    if ( m_ton != NOBLINK && m_toff != NOBLINK ) {
        m_isblinking = true;
        BlinkInfo* bi = new BlinkInfo;
        bi->pin = m_pin;
        bi->pwm = m_pwm;
        bi->max = m_max;
        bi->min = m_min;
        bi->ton = m_ton;
        bi->toff = m_toff;
        bi->time = 0;
        bi->blink = true;
        bi->invert = m_invert;
        g_blink.add( bi);
        startTimerEvent( 1, onBlinkEvent);
    }
}

void Led::setOff()
{
    if ( m_pin < 0 ) return;
    Actuator::setOff();
    m_isblinking = false;
    stopTimerEvent();

    for ( int i = 0; i < g_blink.size(); i++ ) {
        BlinkInfo *blk = g_blink[i];
        if ( blk->pin == m_pin ) {
            g_blink.removeAt( i);
            delete blk;
            break;
        }
    }
    if ( m_pwm )
        analogWrite( m_pin, m_invert ? PINBRIGHT : 0);
    else
        digitalWrite( m_pin, m_invert ? HIGH : LOW);
}
