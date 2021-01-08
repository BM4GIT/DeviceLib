// file:   SystemCore.h
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.

#ifndef SYSTEMCORE_H
#define SYSTEMCORE_H

#include <unistd.h>
#include "ArduinoString.h"

extern pid_t system_async(const char * command, int * infp = NULL, int * outfp = NULL);
extern int pidof( String proc_name);
extern String make_linux_path( String path);

#endif // SYSTEMCORE_H
