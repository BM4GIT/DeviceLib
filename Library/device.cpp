// file:   device.h
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#include "device.h"

///////////////////////////////////
// Global timer thread mechanism //
///////////////////////////////////

TimerEvents     g_te;
bool            g_tb = false;
uint32_t        g_tm = 0;
#ifdef RPI
thread          g_tt;
#endif

void threadWorker()
{
#ifdef RPI
    while ( g_tb ) {
        auto ct = chrono::high_resolution_clock::now();
        for ( int i = 0; i < g_te.size(); i++ ) {
            TimerEvent *te = g_te.at( i);
            uint32_t dt = chrono::duration_cast<chrono::milliseconds>( ct - te->ftime).count();
            if ( dt > te->dtime ) {
                te->event();
                te->ftime = ct;
            }
        }
    }
#else
    g_tm++;
    for ( int i = 0; i < g_te.size(); i++ ) {
        TimerEvent *te = g_te.at( i);
        uint32_t dt = g_tm - te->ftime;
        if ( dt >= te->dtime ) {
            te->event();
            te->ftime = g_tm;
        }
    }
#endif
}

///////////////////////
// TimerThread class //
///////////////////////

TimerThread TT;

TimerThread::TimerThread()
{
}

TimerThread::~TimerThread()
{
}

TimerEvent* TimerThread::startEvent( uint32_t msec, void (*event)())
{
    // add event to the timer event handling
    TimerEvent *te = new TimerEvent;
    te->event = event;
    te->dtime = msec;
#ifdef RPI
    te->ftime = chrono::high_resolution_clock::now();
#else
    te->ftime = 0;
#endif
    g_te.add( te);

    // start timer thread if not running yet
    if ( g_te.size() == 1 ) {
        g_tb = true;
        g_tm = 0;
#ifdef RPI
        g_tt = thread( threadWorker);
#else
        Timer2.init( 1000, threadWorker);
        Timer2.start();
#endif
    }

    return te;
}

void TimerThread::stopEvent( TimerEvent *te)
{
    // remove event from the timer event handling
    for ( int i = 0; i < g_te.size(); i++ )
        if ( g_te.at( i) == te ) {
            g_te.removeAt( i);
            break;
        }
    delete te;

    // stop timer thread if no more timer events
    if ( !g_te.size() ) {
        g_tb = false;
#ifdef RPI
        g_tt.join();
#else
        Timer2.stop();
#endif
    }
}

//////////////////
// Device class //
//////////////////

Device::Device()
{
}

Device::~Device()
{
    stopTimerEvent();
}

void Device::startTimerEvent( uint32_t msec, void (*event)())
{
    if ( m_ei != nullptr )
        stopTimerEvent();
    m_ei = TT.startEvent( msec, event);
}

void Device::stopTimerEvent()
{
    if ( m_ei != nullptr )
        TT.stopEvent( m_ei);
    m_ei = nullptr;
}

uint32_t Device::millisTimer()
{
    return g_tm;
}
