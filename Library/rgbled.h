// file:   rgbled.h
// Copyright 2020 D.E.Repolev
//
// This file is part of BmLib. BmLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: BmLib is distributed without any warranty.


#ifndef RGBLED_H
#define RGBLED_H

#include "led.h"

#define RED     1
#define GREEN   2
#define YELLOW  3
#define BLUE    4
#define MAGENTA 5
#define CYAN    6
#define WHITE   7

class RgbLed : public Actuator
{
public:
    RgbLed();
    ~RgbLed();

    void setPin( uint8_t pinRed,
                 uint8_t pinGreen,
                 uint8_t pinBlue,
                 bool pwm = true);
    void setInversion( bool invert = false);

    void setColor( uint8_t color);
    void setColor( uint8_t red, uint8_t green, uint8_t  blue);
    void setOn();
    void setOff();

    Led* redLed();
    Led* greenLed();
    Led* blueLed();

private:
    Led m_red;
    Led m_green;
    Led m_blue;
    bool m_invert;
};

#endif // RGBLED_H
