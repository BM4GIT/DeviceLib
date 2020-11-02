// file:	hx711.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#include "hx711.h"

namespace HX711 {

Scale::Scale()
{
}

Scale::~Scale()
{
}

void Scale::setPin( uint8_t pinClk, uint8_t pinOut)
{
	m_clk = pinClk;
	m_out = pinOut;

	pinMode( pinClk, OUTPUT);
	pinMode( pinOut, INPUT);

	digitalWrite( pinClk, HIGH);
	delayMicroseconds(100);
	digitalWrite( pinClk, LOW);

	averageValue();
	calibrate( averageValue());
	setScale();
}

void Scale::read()
{
	m_value = averageValue( 25) - m_zero;
	if ( m_value < 0 ) m_value = 0;
	m_value /= m_scale;
	Sensor::read();
}

long Scale::averageValue( uint8_t times)
{
	long sum = 0;
	for ( int i = 0; i < times; i++)
		sum += getValue();

	return sum / times;
}

long Scale::getValue()
{
	uint8_t data[3];

	while ( digitalRead( m_out) );

	for ( int j = 0; j < 3; j++ )
	{
		for ( int i = 0; i < 8; i++ )
		{
			digitalWrite( m_clk, HIGH);
			bitWrite( data[2 - j], 7 - i, digitalRead( m_out));
			digitalWrite( m_clk, LOW);
		}
	}

	digitalWrite( m_clk, HIGH);
	digitalWrite( m_clk, LOW);

	return ((long) data[2] << 16) | ((long) data[1] << 8) | (long) data[0];
}

void Scale::calibrate( long zero)
{
	m_zero = zero;
}

void Scale::setScale(float scale)
{
	m_scale = scale;
}

uint16_t Scale::kg()
{
	return (uint16_t) m_value;
}

} // end namespace
