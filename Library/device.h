// file:   device.h
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#ifndef DEVICE_H
#define DEVICE_H

#ifdef RPI
#include "ArduinoCore.h"
#include "ArduinoString.h"
#include <thread>
#include <chrono>
#else
#include <TimerTwo.h>
#endif
#include "linkedlist.h"

using namespace std;

class Device;

struct TimerEvent
{
    void (*event)();
    uint32_t dtime;
#ifdef RPI
    chrono::time_point<chrono::high_resolution_clock> ftime;
#else
    uint32_t ftime;
#endif
};

typedef LinkedList <TimerEvent*> TimerEvents;

class TimerThread
{
public:

    TimerThread();
    ~TimerThread();

    TimerEvent* startEvent( uint32_t msec, void (*event)());
    void stopEvent( TimerEvent* te);

protected:

};

class Device
{
public:

    Device();
    ~Device();

    // event handling
    void startTimerEvent( uint32_t msec, void (*event)());
    void stopTimerEvent();
    uint32_t millisTimer();

protected:

    TimerEvent *m_ei;
};

extern TimerThread TT;

#endif
