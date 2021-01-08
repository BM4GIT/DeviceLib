// file:   sound.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#ifdef RPI

#include "sound.h"
#include "SystemCore.h"
#include <thread>

thread g_play;
bool g_stop;
int g_volume = LEVEL_3;

LinkedList<String>  g_item; // audio-file or spoken text
LinkedList<uint8_t> g_type; // 0 = audio-file, 1 = spoken text
int g_current = -1;

void play_file( String path, uint8_t streamer, String device, bool async)
{
    String play;
    switch ( streamer ) {
        case LOCAL :        play = "omxplayer -o local --vol "; play += String( g_volume, DEC) + " "; break;
        case HDMI :         play = "omxplayer -o hdmi --vol "; play += String( g_volume, DEC) + " "; break;
        case BLUETOOTH :    play = String( "aplay -D bluealsa:HCI=hci0,DEV=") + device + String( ",PROFILE=a2dp "); break;
    }
	play += make_linux_path( path);
    if ( async )
        system_async( play.c_str());
    else
        system( play.c_str());
}

void play_text( String text, uint8_t  streamer, String device, bool async)
{
	String speak = "espeak \"[][][][] ";
    speak += text;
    switch ( streamer ) {
        case LOCAL :
        case HDMI :         speak += "\" --stdout | aplay"; break;
        case BLUETOOTH :    speak += String( "\" -a20 -ven+f2 -k5 -s170 --stdout | aplay -D bluealsa:HCI=hci0,DEV=")
                                        + device + String( ",PROFILE=a2dp");
    }
    if ( async )
        system_async( speak.c_str());
    else
        system( speak.c_str());
}

void play_list( uint8_t streamer, String device)
{
    g_stop = false;
    g_current = -1;
    while ( g_current < g_item.size() ) {
        g_current++;
        if ( g_type[g_current] )
            play_text( g_item[g_current], streamer, device, false);
        else {
            if ( g_stop ) break;
            play_file( g_item[g_current], streamer, device, false);
        }
    }
}


/////////////////
// CLASS SOUND //
/////////////////

void Sound::init( uint8_t streamer, String device)
{
    m_streamer = streamer;
    m_device = device;
}

void Sound::setVolume( int milli_db)
{
   g_volume = milli_db;
}

uint8_t Sound::addFile( String audio_file)
{
    uint8_t ix = g_item.add( audio_file);
    g_type.add( false);
    return ix;
}

uint8_t Sound::addText( String text)
{
    uint8_t ix = g_item.add( text);
    g_type.add( true);
    return ix;
}

void Sound::insertFile( uint8_t pos, String audio_file)
{
    g_item.insert( pos, audio_file);
    g_type.insert( pos, false);
}

void Sound::insertText( uint8_t pos, String text)
{
    g_item.insert( pos, text);
    g_type.insert( pos, true);
}

void Sound::remove( uint8_t pos, uint8_t count)
{
    for ( int i = pos; i < pos + count; i++ ) {
        g_item.removeAt( i);
        g_type.removeAt( i);
    }
}

void Sound::clear()
{
    g_item.removeAll();
    g_type.removeAll();
}

void Sound::playList( bool async)
{
    g_current = 0;
    if ( async )
        g_play = thread( play_list, m_streamer, m_device);
    else
        play_list( m_streamer, m_device);
}

void Sound::playNext()
{
    if ( g_current < g_item.size() -1 )
        stopplaying(); // play_list will play next after stop
}

void Sound::playCurrent()
{
    g_current--;
    stopplaying(); // play_list will play next after stop
}

void Sound::playPrevious()
{
    if ( g_current > 0 ) {
        g_current -= 2; // play_list will play next after stop
        stopplaying();
    }
}

void Sound::playFile( String path, bool async)
{
    play_file( path, m_streamer, m_device, async);
}

void Sound::playText( String text, bool async)
{
    play_text( text, m_streamer, m_device, async);
}

void Sound::stopplaying()
{
    int pid = pidof( "omxplayer.bin");
    String cmd = String( "sudo kill ") + String( pid, DEC);
    system( cmd.c_str());
//    kill( pid, SIGKILL);
}

void Sound::stop()
{
    g_stop = true;
    stopplaying();
    g_play.join();
}

#endif

