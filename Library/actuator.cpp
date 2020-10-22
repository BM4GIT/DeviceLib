// file:   actuator.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of BmLib. BmLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: BmLib is distributed without any warranty.


#include "actuator.h"

Actuator::Actuator()
{
    m_ison = false;
}

Actuator::~Actuator()
{
}

void Actuator::setOn()
{
    m_ison = true;
}

void Actuator::setOff()
{
    m_ison = false;
}

bool Actuator::isOn()
{
    return m_ison;
}
