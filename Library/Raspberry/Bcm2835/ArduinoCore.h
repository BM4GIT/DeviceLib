// ArduinoCore.h

/*
*  Copyright (C) 2012 Libelium Comunicaciones Distribuidas S.L.
*  http://www.libelium.com
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Version 2.4 (For Raspberry Pi 2)
*  Author: Sergio Martinez, Ruben Martin
*
*  Version BmLib, 2020 (For BmLib library on Raspberry)
*  The complete library has been split up to mirror the arduino configuration.
*  These are the components in the BmLib library:
*  > BcmCore		- all helper stuff
*  > ArduinoCore	- global routines like pinMode, digitalWrite, etc. and the Serial interface
*  > Wire			- the I2C interface
*  > SPI			- the SPI interface
*  Some minor changes have been made to meet the structure of the BmLib library.
*/

#ifndef ARDUINOCORE_H
#define ARDUINOCORE_H

#include <thread>
#include <chrono>
#include "BcmCore.h"
#include "ArduinoString.h"

using namespace std;

#define sprint			Serial.print
#define sprintln		Serial.println
#define sflush			Serial.flush

typedef chrono::time_point<chrono::high_resolution_clock>	hirestime;
extern hirestime currentTime();
extern void localTime( tm &lt, uint32_t &msec);
extern ulong diffMillis( hirestime stop, hirestime start);
extern ulong diffMicros( hirestime stop, hirestime start);

#define OUTPUT			0
#define INPUT			1
#define INPUT_PULLUP	2
#define INPUT_PULLDOWN	3

//#define LOW			0
//#define HIGH			1
#define RISING			2
#define FALLING			3
#define CHANGE			4

typedef uint8_t byte;

// GENERAL ARDUINO FUNCTIONS

void pinMode( int pin, int mode);
#define digitalWrite( pin, state)	bcm2835_gpio_write( pin, state)
#define digitalRead( pin)			bcm2835_gpio_lev( pin)
void analogWrite( int pin, int level);
int analogRead (int pin);
void attachInterrupt(int p,void (*f)(), uint m);
void detachInterrupt(int p);

long millis();
long micros();

struct ThreadArg{
    void (*func)();
    int pin;
};

// Helper functions
pthread_t *getThreadIdFromPin(int pin);
void * threadFunction(void *args);

// ARDUINO SERIAL INTERFACE

#define SERIAL_5N1	0
#define SERIAL_6N1	1
#define SERIAL_7N1	2
#define SERIAL_8N1	3
#define SERIAL_5N2	4
#define SERIAL_6N2	5
#define SERIAL_7N2	6
#define SERIAL_8N2	7
#define SERIAL_5E1	8
#define SERIAL_6E1	9
#define SERIAL_7E1	10
#define SERIAL_8E1	11
#define SERIAL_5E2	12
#define SERIAL_6E2	13
#define SERIAL_7E2	14
#define SERIAL_8E2	15
#define SERIAL_5O1	16
#define SERIAL_6O1	17
#define SERIAL_7O1	18
#define SERIAL_8O1	19
#define SERIAL_5O2	20
#define SERIAL_6O2	21
#define SERIAL_7O2	22
#define SERIAL_8O2	23

// ARDUINO SERIAL MONITOR

class SerialRPi
{
public:

	SerialRPi( bool isconsole);

	uint available();
	uint availableForWrite();

	void begin( const ulong baud, const uint config = SERIAL_8N1);
	void end(); 

	bool find( const String search);
	bool findUntil( const String search, const String stop = "");

	void flush();

	double parseFloat();
	long   parseInt();

	int8_t peek();

	uint print( const int i, const NUMSYSTEM type = DEC);
	uint print( const double f, const int decimals = 2);
	uint print( const String s = "");
	uint println( const int i, const NUMSYSTEM type = DEC);
	uint println( const double f, const int decimals = 2);
	uint println( const String s = "");

	int8_t read();
	byte   readBytes( byte data[], const byte len);
	byte   readBytesUntil( const char stop, byte data[], const byte len);
	String readString();
	String readStringUntil( const char stop);

	void setTimeout( ulong millis);

	uint write( const byte data);
	uint write( const String str);
	uint write( const byte buf[], const uint len);

private:

	byte readB( const char stop, byte data[], const byte len, bool dostop);
	String readS( const char stop, bool dostop);

	bool   m_console;
	bool   m_enabled;
	ulong  m_timeout;
	int    m_uart;
};

extern SerialRPi Serial;
extern SerialRPi Serial1;

#endif // ARDUINOCORE_H
