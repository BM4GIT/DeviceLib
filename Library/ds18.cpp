// file:   dst.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of BmLib. BmLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: BmLib is distributed without any warranty.


#include "ds18.h"

#ifdef RPI
#include <dirent.h>
#include <fstream>
#else
#include "OneWire.h"

typedef uint8_t DeviceAddress[8];

OneWire w1;
LinkedList<uint8_t*> g_dalist;
int	g_parasite;

// Model IDs
#define DS18S20MODEL 0x10  // also DS1820
#define DS18B20MODEL 0x28
#define DS1822MODEL  0x22
#define DS1825MODEL  0x3B
#define DS28EA00MODEL 0x42

// Scratchpad locations
#define TEMP_LSB        0
#define TEMP_MSB        1
#define HIGH_ALARM_TEMP 2
#define LOW_ALARM_TEMP  3
#define CONFIGURATION   4
#define INTERNAL_BYTE   5
#define COUNT_REMAIN    6
#define COUNT_PER_C     7
#define SCRATCHPAD_CRC  8

// Error Codes
#define DEVICE_DISCONNECTED_C -127
#define DEVICE_DISCONNECTED_F -196.6
#define DEVICE_DISCONNECTED_RAW -7040

// OneWire commands
#define STARTCONVO      0x44  // Tells device to take a temperature reading and put it on the scratchpad
#define COPYSCRATCH     0x48  // Copy EEPROM
#define READSCRATCH     0xBE  // Read EEPROM
#define WRITESCRATCH    0x4E  // Write to EEPROM
#define RECALLSCRATCH   0xB8  // Reload from last known
#define READPOWERSUPPLY 0xB4  // Determine if device needs parasite power
#define ALARMSEARCH     0xEC  // Query bus for devices with an alarm condition

#endif // RPI else

namespace DS18
{

Temperature::Temperature()
{
    m_pin = 10;
    m_temp = 0;
#ifndef RPI
	g_parasite = false;
#else
    m_id = -1;
#endif
}

StringList Temperature::getIdList( uint8_t pin)
{
	StringList idlist;

#ifdef RPI
	DIR *dir;
	struct dirent *ent;


	if ( (dir = opendir( "/sys/bus/w1/devices")) != NULL ) {
		while ( (ent = readdir( dir)) != NULL )
			idlist.add( ent->d_name);
		closedir( dir);
	}
    for( int ix = idlist.size() - 1; ix >= 0; ix-- )
        if ( idlist[ix].substring( 0, 3) != "28-" )
            idlist.removeAt( ix);
#else
	uint8_t deviceAddress[8];
	uint8_t *da;
	w1.set_pin( pin);
	w1.reset_search();
	while ( w1.search( deviceAddress) ) {

		if ( w1.crc8( deviceAddress, 7) == deviceAddress[7] ) {
			if ( !g_parasite ) {
				w1.reset();
				w1.select( deviceAddress);
				w1.write( READPOWERSUPPLY);
				if (w1.read_bit() == 0 )
					g_parasite = true;
				w1.reset();
			}

			switch ( deviceAddress[0] ) {
				case DS18S20MODEL:
				case DS18B20MODEL:
				case DS1822MODEL:
				case DS1825MODEL:
				case DS28EA00MODEL:
					da = new DeviceAddress;
					memcpy( da, &deviceAddress, sizeof( deviceAddress));
					g_dalist.add( da);
					break;
			}	
		}
	}

    for ( int i = 0; i < g_dalist.size(); i++ ) {
        String id;
        uint8_t* da = g_dalist.at( i);
        for ( int j = 0; j < 8; j++ ) {
            id += String( (int) da[j], HEX);
            if ( j < 7 ) id += " ";
        }
        idlist.add( id);
    }
#endif

    return idlist;
}

void Temperature::setSensor( String id, uint8_t pin)
{
    StringList idlist = getIdList();
#ifdef RPI
    m_id = id;
    if ( m_id == "" ) {
        // set to first dallas id
        if ( idlist.size() > 0 )
            m_id = idlist.at( 0);
        return;
    }
#else
    m_pin = pin;
    if ( id == "" && idlist.size() > 0 ) {
        m_id = 0; // set to first dallas id
        return;
    }
    for ( m_id = 0; m_id < idlist.size(); m_id++ )
        if ( idlist.at( m_id) == id ) break;
#endif
}

void Temperature::read()
{
	m_temp = 0;

#ifdef RPI
    if ( m_id == "" )
        return;
    String fn = String( "/sys/bus/w1/devices/") + m_id + String( "/w1_slave");
	ifstream in;
	in.open( fn.c_str());
	if ( in.is_open() ) {
        String value;
        char val = '!';
        while ( !in.eof() && val != 0  ) {
            in >> val;
            value += val;
        }
        int ix = value.lastIndexOf( "t=");
        if ( ix < 0 ) {
            m_temp = 0;
            return;
        }
        else
            m_temp = value.substring( ix + 2).toFloat() / 10000 + 273;

    }
    else {
        m_temp = 0;
        return;
    }
#else
    if ( m_id < 0 || m_id >= g_dalist.size() )
        return;
	uint8_t* deviceAddress = g_dalist.at( m_id);

    // read sensor;
	if ( !w1.reset() )
        return;
	w1.select( deviceAddress);
	w1.write( STARTCONVO, g_parasite);
	delay( 750); // assuming largest (= 12) bit resolution

	if ( !w1.reset() )
        return;
	w1.select( deviceAddress);
	w1.write( READSCRATCH);

	uint8_t scratchPad[9];
	for ( int i = 0; i < 9; i++ )
		scratchPad[i] = w1.read();

	if ( !w1.reset() )
        return;

	int16_t rawTemp = (((int16_t) scratchPad[TEMP_MSB]) << 11)
                      | (((int16_t) scratchPad[TEMP_LSB]) << 3);
	if ( deviceAddress[0] == DS18S20MODEL ) {
		rawTemp = ((rawTemp & 0xfff0) << 3) - 16
                        + (((scratchPad[COUNT_PER_C] - scratchPad[COUNT_REMAIN]) << 7)
                        / scratchPad[COUNT_PER_C]);
	}

	if ( rawTemp <= DEVICE_DISCONNECTED_RAW )
        return;

	m_temp = (float) rawTemp * 0.0078125 + 273;
#endif

    Sensor::read();
}

float Temperature::kelvin()
{
    return m_temp;
}

float Temperature::celcius()
{
    return m_temp - 273;
}

float Temperature::fahrenheit()
{
    return (m_temp - 273) * 9 / 5 + 32;
}

} // end namespace
