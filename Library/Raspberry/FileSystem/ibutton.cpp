// file:   ibutton.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of BmLib. BmLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: BmLib is distributed without any warranty.


#include "ibutton.h"
#include "stdio.h"

const String NOBUTTON = "No button";
const String EXPIRED = "Validation expired";
const String INVALID = "Invalid CRC";
String BASESIR = "/sys/devices/w1_bus_master1/w1_master_slaves";
String REMOVEDIR = "/sys/devices/w1_bus_master1/w1_master_remove";

IButton::IButton()
{
}

IButton::~IButton()
{
}

void IButton::setPin( uint8_t pin) // dummy to keep in line with arduino
{
}

void IButton::setTimeout( uint32_t timeout)
{
    m_timeout = timeout;
}

void IButton::read()
{
    char id[50];
    FILE* fp;
    uint32_t tm = millis();
    m_id = NOBUTTON;
    while ( millis() - tm < m_timeout ) {
        delay( 50);
        fp = fopen( BASESIR.c_str(), "r");
        if ( !fp ) return;
        fgets( id, 50, fp);
        fclose( fp);
        if ( String( id) != "not found.\n" ) {
            // remove button from w1 system entries
            fp = fopen( REMOVEDIR.c_str(), "w");
            if ( !fp ) return;
            fputs( id, fp);
            fclose( fp);
            bool isnull = true;
            for ( int i = 0; i < String( id).length(); i++ )
                if ( id[i] != '0' && id[i] != ' ' )
                    isnull = false;
            if ( !isnull ) {
                m_id = id;
                Sensor::read();
                m_stamp = millis();
            }
        }
        break;
    }
}

String IButton::tag( uint32_t expireAfter)
{
    String id;
    if ( expireAfter && ((uint32_t)millis() > m_stamp + expireAfter) )
        id = EXPIRED;
    else
        id = m_id;
    return id;
}
