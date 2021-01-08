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

// audio devices
#define LOCAL       0
#define HDMI        1
#define BLUETOOTH   2

// volume levels
#define LEVEL_5     800
#define LEVEL_4     400
#define LEVEL_3     -100
#define LEVEL_2     -700
#define LEVEL_1     -1400

class Sound : public Actuator
{
public:

    Sound() { m_streamer = LOCAL; m_stop = false; };
    ~Sound() {};

    void init( uint8_t streamer, String device = "");

    void setVolume( int milli_db);

    uint8_t addFile( String audio_file);
    uint8_t addText( String text);
    void insertFile( uint8_t pos, String audio_file);
    void insertText( uint8_t pos, String text);
    void remove( uint8_t pos, uint8_t count = 1);
    void clear();

    void playList( bool async = true);                  // plays the list
    void playNext();
    void playCurrent(); // (means restart current)
    void playPrevious();

    void playFile( String path, bool async = false);    // calls 'omxplayer' or 'aplay' to play the file
    void playText( String text, bool async = false);    // calls 'espeak' to say the text
    void stop();

protected:

    void stopplaying();

    uint8_t     m_streamer;
    String      m_device;
    bool        m_stop;
};

#endif // RPI

#endif // Sound_H
