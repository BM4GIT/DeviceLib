// file:   average.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.

#include "average.h"

Average::Average()
{
    clear();
}

void Average::clear()
{
    m_cur = 0;
    m_complete = false;
}

void Average::add( float value)
{
    m_val[m_cur] = value;
    m_cur++;
    if ( m_cur == 20 ) {
        m_cur = 0;
        m_complete = true;
    }
}

float Average::avg()
{
    float sum = 0;
    int x = (m_complete ? 20 : m_cur + 1);
    for ( int i = 0; i < x; i++ )
        sum += m_val[i];
    return sum / x + 0.5;
}
