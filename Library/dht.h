// file:   dht.h
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#ifndef DHT_H
#define DHT_H

// Max sample rate DHT11 is 1 Hz   (duty cicle 1000 ms)
// Max sample rate DHT22 is 0.5 Hz (duty cicle 2000 ms)

#include "sensor.h"

namespace DHT
{

extern const char* DHT11;
extern const char* DHT22;

class Temperature : public Sensor
{
public:

    Temperature();
    ~Temperature() {};

    void setSensor( String id, uint8_t pin = 10);
    void read();

    float kelvin();
    float celcius();
    float fahrenheit();

    float humidity();

private:

    bool readDHT();

    String  m_id;
    int8_t  m_pin;
    float   m_temp;
    float   m_hum;
};

} //end namespace

#endif
