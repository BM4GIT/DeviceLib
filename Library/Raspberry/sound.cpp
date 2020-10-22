// file:   sound.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of BmLib. BmLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: BmLib is distributed without any warranty.


#ifdef RPI

#include "sound.h"

void Sound::init( Streamer streamer, String device)
{
    m_streamer = streamer;
    m_device = device;
}

uint8_t Sound::add( String sound, bool astext)
{
    uint8_t ix = m_item.add( sound);
    m_type.add( astext);
    return ix;
}

void Sound::insert( uint8_t pos, String sound, bool astext)
{
    m_item.insert( pos, sound);
    m_type.insert( pos, astext);
}

void Sound::remove( uint8_t pos, uint8_t count)
{
    for ( int i = pos; i < pos + count; i++ ) {
        m_item.removeAt( i);
        m_type.removeAt( i);
    }
}

void Sound::clear()
{
    m_item.removeAll();
    m_type.removeAll();
}

void Sound::playList()
{
    for (int i = 0; i < m_item.size(); i++ )
        if ( m_type[i] )
            playText( m_item[i]);
        else
            playFile( m_item[i]);
}

void Sound::playFile( String path)
{
    String play;
    switch ( m_streamer ) {
        case Local :        play = "omxplayer -o local "; break;
        case HDMI :         play = "omxplayer -o hdmi "; break;
        case Bluetooth :    play = String( "aplay -D bluealsa:HCI=hci0,DEV=") + m_device + String( ",PROFILE=a2dp "); break;
    }
	play += path;
	system( play.c_str());
}

void Sound::playText( String text)
{
	String speak = "espeak \"[][][][] ";
    speak += text;
    switch ( m_streamer ) {
        case Local :
        case HDMI :         speak += "\" --stdout | aplay"; break;
        case Bluetooth :    speak += String( "\" -a20 -ven+f2 -k5 -s170 --stdout | aplay -D bluealsa:HCI=hci0,DEV=")
                                        + m_device + String( ",PROFILE=a2dp");
    }
	system( speak.c_str());
}

#endif

