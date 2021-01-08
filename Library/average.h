// file:   average.h
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.

#ifndef AVERAGE_H
#define AVERAGE_H

class Average
{
public:
    Average();

    void  add( float value);
    void  clear();
    float avg();

protected:
    float  m_val[20];
    int    m_cur;
    bool   m_complete;
};

#endif // AVERAGE_H
