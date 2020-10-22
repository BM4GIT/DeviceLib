// file:   beeper.h
// Copyright 2020 D.E.Repolev
//
// This file is part of BmLib. BmLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: BmLib is distributed without any warranty.


#ifndef BEEPER_H
#define BEEPER_H

#include "actuator.h"
#include "linkedlist.h"

class Beeper : public Actuator
{
public:
    Beeper();
    ~Beeper();

    void setPin( uint8_t pin);

    void setBeeper( float herz, uint16_t msec);

    void beep();

    uint8_t addTone( float herz, uint16_t msec);
    void insertTone( uint8_t pos, float herz, uint16_t msec);
    void remove( uint8_t pos, uint8_t count = 1);
    void clear();

    void play();    // plays the tones

    void setOn();   // activates the beeper
    void setOff();

protected:

    void playtone( float herz, uint16_t msec);

    float                   m_beep;
    uint16_t                m_dur;

    int8_t                  m_pin;
    LinkedList<float>       m_hz;
    LinkedList<uint16_t>    m_tm;
};

#define     WHOLE_NOTE       2800
#define     HALF_NOTE        1400
#define     QUARTER_NOTE     700
#define     EIGHTH_NOTE      350
#define     SIXTEENTH_NOTE   175

#define     PIANO_C0        16.352
#define     PIANO_C0s       17.324
#define     PIANO_D0        18.354
#define     PIANO_D0s       19.445
#define     PIANO_E0        20.602
#define     PIANO_F0        21.827
#define     PIANO_F0s       23.125
#define     PIANO_G0        24.500
#define     PIANO_G0s       25.957
#define     PIANO_A0        27.500
#define     PIANO_A0s       29.135
#define     PIANO_B0        30.868

#define     PIANO_C1        32.703
#define     PIANO_C1s       34.648
#define     PIANO_D1        36.708
#define     PIANO_D1s       38.891
#define     PIANO_E1        41.203
#define     PIANO_F1        43.654
#define     PIANO_F1s       46.249
#define     PIANO_G1        48.999
#define     PIANO_G1s       51.913
#define     PIANO_A1        55.000
#define     PIANO_A1s       58.270
#define     PIANO_B1        61.735

#define     PIANO_C2        65.406
#define     PIANO_C2s       69.296
#define     PIANO_D2        73.416
#define     PIANO_D2s       77.782
#define     PIANO_E2        82.407
#define     PIANO_F2        87.307
#define     PIANO_F2s       92.499
#define     PIANO_G2        97.999
#define     PIANO_G2s       103.83
#define     PIANO_A2        110.00
#define     PIANO_A2s       116.54
#define     PIANO_B2        123.47

#define     PIANO_C3        130.81
#define     PIANO_C3s       138.59
#define     PIANO_D3        146.83
#define     PIANO_D3s       155.56
#define     PIANO_E3        164.81
#define     PIANO_F3        174.61
#define     PIANO_F3s       185.00
#define     PIANO_G3        196.00
#define     PIANO_G3s       207.65
#define     PIANO_A3        220.00
#define     PIANO_A3s       233.08
#define     PIANO_B3        246.94

#define     PIANO_C4        261.63
#define     PIANO_C4s       277.18
#define     PIANO_D4        293.67
#define     PIANO_D4s       311.13
#define     PIANO_E4        329.63
#define     PIANO_F4        349.23
#define     PIANO_F4s       369.99
#define     PIANO_G4        392.00
#define     PIANO_G4s       415.31
#define     PIANO_A4        440.00
#define     PIANO_A4s       466.16
#define     PIANO_B4        493.88

#define     PIANO_C5        523.25
#define     PIANO_C5s       554.37
#define     PIANO_D5        587.33
#define     PIANO_D5s       622.25
#define     PIANO_E5        659.26
#define     PIANO_F5        698.46
#define     PIANO_F5s       739.99
#define     PIANO_G5        783.99
#define     PIANO_G5s       830.61
#define     PIANO_A5        880.00
#define     PIANO_A5s       932.33
#define     PIANO_B5        987.77

#define     PIANO_C6        1046.5
#define     PIANO_C6s       1108.7
#define     PIANO_D6        1174.7
#define     PIANO_D6s       1244.5
#define     PIANO_E6        1318.5
#define     PIANO_F6        1396.9
#define     PIANO_F6s       1480.0
#define     PIANO_G6        1568.0
#define     PIANO_G6s       1661.2
#define     PIANO_A6        1760.0
#define     PIANO_A6s       1864
#define     PIANO_B6        1975

#define     PIANO_C7        2093
#define     PIANO_C7s       2217
#define     PIANO_D7        2349
#define     PIANO_D7s       2489
#define     PIANO_E7        2637
#define     PIANO_F7        2793
#define     PIANO_F7s       2959
#define     PIANO_G7        3135
#define     PIANO_G7s       3322
#define     PIANO_A7        3520
#define     PIANO_A7s       3729
#define     PIANO_B7        3951

#define     PIANO_C8        4186
#define     PIANO_C8s       4434
#define     PIANO_D8        4698
#define     PIANO_D8s       4978
#define     PIANO_E8        5274
#define     PIANO_F8        5587
#define     PIANO_F8s       5919
#define     PIANO_G8        6271
#define     PIANO_G8s       6644
#define     PIANO_A8        7040
#define     PIANO_A8s       7458
#define     PIANO_B8        7902

#define     PIANO_C9        8372
#define     PIANO_C9s       8869
#define     PIANO_D9        9397
#define     PIANO_D9s       9956
#define     PIANO_E9        10548
#define     PIANO_F9        11175
#define     PIANO_F9s       11839
#define     PIANO_G9        12543
#define     PIANO_G9s       13289
#define     PIANO_A9        14080
#define     PIANO_A9s       14917
#define     PIANO_B9        15804

#endif // Beeper_H
