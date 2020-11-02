// file:   sound.h
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#ifndef SOUND_H
#define SOUND_H

#ifndef RPI
#error NOTE: You cannot use 'sound.h' on Arduino
#else

#include "actuator.h"
#include "linkedlist.h"

class Sound : public Actuator
{
public:
    enum Streamer { Local, HDMI, Bluetooth };

    Sound() { m_streamer = Local; };
    ~Sound() {};

    void init( Streamer streamer, String device = "");

    uint8_t add( String sound, bool astext = false);
    void insert( uint8_t pos, String sound, bool astext = false);
    void remove( uint8_t pos, uint8_t count = 1);
    void clear();

    void playList();                // plays the list
    void playFile( String path);    // calls 'aplay' to play the file
    void playText( String text);    // calls 'espeak' to say the text

protected:

    LinkedList<String>  m_item; // audio-file or spoken text
    LinkedList<uint8_t> m_type; // 0 = audio-file, 1 = spoken text
    Streamer    m_streamer;
    String      m_device;
};

#endif // RPI

#endif // Sound_H
