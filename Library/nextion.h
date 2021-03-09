// file:   nextion.ch
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.

#ifndef NEXTION_H
#define NEXTION_H

#ifdef RPI
#include "ArduinoCore.h"
#include "ArduinoString.h"
#else
#include <Arduino.h>
#endif

class Nextion
{
public:
    Nextion();
    ~Nextion();

#ifdef RPI
    void init( SerialRPi& serial);
#else
    void init( HardwareSerial& serial);
#endif

    String  receive();
	// this routine receives a Nextion print instruction
	// it must be:
	// - a text ending by a ~ sign, or
	// - a 4 byte number (little endian) prefixed by a # sign

    void    sendPage( String page);
    void    sendNumber( String field, long data);
    void    sendText( String field, String data);

private:
    void    send( String data);

    void*   m_serial;
};

#endif // NEXTION_H
