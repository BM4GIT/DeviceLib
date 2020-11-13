// file:   nextion.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.

#include "nextion.h"

Nextion::Nextion()
{
    m_serial = NULL;
}

Nextion::~Nextion()
{
#ifdef RPI
    ((SerialRPi*) m_serial)->end();
#endif
}

#ifdef RPI
void Nextion::init( SerialRPi& serial)
#else
void Nextion::init( HardwareSerial& serial)
#endif
{
    m_serial = &serial;
    serial.begin( 9600);
}

String Nextion::receive()
{
    String data;
#ifdef RPI
    if ( ((SerialRPi*) m_serial)->available() ) {
        char c = (char) ((SerialRPi*) m_serial)->read();
#else
    if ( ((HardwareSerial*) m_serial)->available() ) {
        char c = (char) ((HardwareSerial*) m_serial)->read();
#endif
        if ( c == '#' ) {
            int val = 0;
            for ( int i = 0; i < 4; i++ ) {
                // 4 bytes in little endian order
#ifdef RPI
                int v = ((SerialRPi*) m_serial)->read();
#else
                int v = ((HardwareSerial*) m_serial)->read();
#endif
                val += v << (8 * i);
            }
            data = val;
        }
        else {
            String str;
            while ( c != '~' ) {
                str += c;
#ifdef RPI
                c = (char) ((SerialRPi*) m_serial)->read();
#else
                c = (char) ((HardwareSerial*) m_serial)->read();
#endif
            }
            data = str;
        }
    }

    return data;
}

void Nextion::sendPage( String page)
{
    send( String( "page ") + page);
}

void Nextion::sendNumber( String field, long data)
{
    String str( data, DEC);
    send( field + String( ".val=") + str);
}

void Nextion::sendText( String field, String data)
{
    send( field + String( ".txt=") + data);
}

void Nextion::send( String data)
{
#ifdef RPI
    for ( int i = 0; i < data.length(); i++ )
        ((SerialRPi*) m_serial)->write( (uint8_t) data[i]);
    for ( int i = 0; i < 3; i++ )
        ((SerialRPi*) m_serial)->write( (uint8_t) 0xFF);
#else
    for ( int i = 0; i < data.length(); i++ )
        ((HardwareSerial*) m_serial)->write( (uint8_t) data[i]);
    for ( int i = 0; i < 3; i++ )
        ((HardwareSerial*) m_serial)->write( (uint8_t) 0xFF);
#endif
}
