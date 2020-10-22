// file:   ibutton.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of BmLib. BmLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: BmLib is distributed without any warranty.


#include "ibutton.h"

const String NOBUTTON = "No button";
const String EXPIRED = "Validation expired";
const String INVALID = "Invalid CRC";

int IButton::isPresent( uint8_t pin)
{
    pinMode( pin, OUTPUT);
    digitalWrite( pin, LOW);
    delayMicroseconds( 480);
    pinMode( pin, INPUT);
    delayMicroseconds( 70);
    uint8_t b = digitalRead( pin);
    delayMicroseconds( 410);
    return !b;
}

void IButton::writeOneBit( uint8_t pin, int b)
{
    int delay1, delay2;
    if ( b == 1 ) {
        delay1 = 6;
        delay2 = 64;
    }
    else {
        delay1 = 80;
        delay2 = 10;
    }
    pinMode( pin, OUTPUT);
    digitalWrite( pin, LOW);
    delayMicroseconds( delay1);
    pinMode( pin, INPUT);
    delayMicroseconds( delay2);
}

void IButton::writeOneByte( uint8_t pin, int byte)
{
    for ( int i = 0; i < 8; i++ ) {
        writeOneBit( pin, byte & 1);
        byte = byte >> 1;
    }
}

uint8_t IButton::readOneBit( uint8_t pin)
{
    pinMode( pin, OUTPUT);
    digitalWrite( pin, LOW);
    delayMicroseconds( 6);
    pinMode( pin, INPUT);
    delayMicroseconds( 8);
    uint8_t b = digitalRead( pin);
    delayMicroseconds( 50);
    return b;
}

uint8_t IButton::readOneByte( uint8_t pin)
{
    int byte = 0;
    for ( int i = 0; i < 8; i++ )
        byte |= readOneBit( pin) << i;
    return byte;
}

bool IButton::crc8( uint8_t* data, uint8_t len)
{
    uint8_t temp, databyte;
    uint8_t crc = 0;
    bool isnull = true;
    for ( int i = 0; i < len; i++ ) {
        databyte = data[i];
        if ( databyte ) isnull = false;
        for ( int j = 0; j < 8; j++ ) {
            temp = (crc ^ databyte) & 0x01;
            crc >>= 1;
            if ( temp ) crc ^= 0x8C;
            databyte >>= 1;
        }
    }
    return ( !isnull && (crc == 0) );
}

IButton::IButton()
{
    m_pin = 17;
    m_stamp = 0;
    m_timeout = 500;
}

IButton::~IButton()
{
}

void IButton::setPin( unsigned int pin)
{
    m_pin = pin;
}

void IButton::setTimeout( int timeout)
{
    m_timeout = timeout;
}

void IButton::read()
{
    uint32_t tm = millis();
    uint8_t code[8];
    m_id = NOBUTTON;
    while ( (uint32_t)(millis() - tm) < m_timeout ) {
        if ( isPresent( m_pin) ) {
            writeOneByte( m_pin, 0x33);
            for ( int i = 0; i < 8; i++ )
                code[i] = readOneByte( m_pin);
            if ( !crc8( code, 8) ) {
                m_id = INVALID;
                break;
            }
            m_id = "";
            for ( int i = 7; i > 0 ; i-- ) {
                if ( m_id != "" ) m_id += " ";
                if ( code[i] < 16) m_id += "0";
                m_id += String( code[i], HEX) + " ";
            }
            if ( code[0] < 16) m_id += "0";
            m_id += String( code[0], HEX);
            m_stamp = millis();
            Sensor::read();
            break;
        }
    }
}

String IButton::tag( long expireAfter)
{
    String id;
    if ( expireAfter && (millis() > m_stamp + expireAfter) )
        id = EXPIRED;
    else
        id = m_id;
    return id;
}
