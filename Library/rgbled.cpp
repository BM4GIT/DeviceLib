// file:   rgbled.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#include "rgbled.h"

RgbLed::RgbLed()
{
	m_invert = true;
	m_red.setInversion();
	m_green.setInversion();
	m_blue.setInversion();
}

RgbLed::~RgbLed()
{
    off();
}

void RgbLed::setPin( uint8_t pinRed,
                     uint8_t pinGreen,
                     uint8_t pinBlue,
                     bool pwm)
{
	m_red.setPin( pinRed, pwm);
	m_green.setPin( pinGreen, pwm);
	m_blue.setPin( pinBlue, pwm);
}

void RgbLed::setInversion( bool invert)
{
	m_red.setInversion( invert);
	m_green.setInversion( invert);
	m_blue.setInversion( invert);
}

void RgbLed::setColor( uint8_t color)
{
    uint8_t red = (color & RED ? BRIGHT : 0);
    uint8_t green = (color & GREEN ? BRIGHT : 0);
    uint8_t blue = (color & BLUE ? BRIGHT : 0);
    setColor( red, green, blue);
}

void RgbLed::setColor( uint8_t red, uint8_t green, uint8_t blue)
{
	if ( red > BRIGHT ) red = BRIGHT;
	if ( green > BRIGHT ) green = BRIGHT;
	if ( blue > BRIGHT ) blue = BRIGHT;
	m_red.setBrightness( red);
	m_green.setBrightness( green);
	m_blue.setBrightness( blue);
}

void RgbLed::on()
{
	Actuator::on();
	m_red.on();
	m_green.on();
	m_blue.on();
}

void RgbLed::off()
{
	Actuator::off();
	m_red.off();
	m_green.off();
	m_blue.off();
}


Led* RgbLed::redLed()
{
	return &m_red;
}

Led* RgbLed::greenLed()
{
	return &m_green;
}

Led* RgbLed::blueLed()
{
	return &m_blue;
}
