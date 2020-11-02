// file:    ArduinoString.h
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#ifndef ARDUINOSTRING_H
#define ARDUINOSTRING_H

#include <cstring>
#include <string>

using namespace std;

enum NUMSYSTEM { DEC, HEX, OCT, BIN, BYTE };

class String
{
public:

	String();
	String( const String &str);
	String( const char chr);
	String( const char* str);
	String( const string &str);
	String( const int val, const NUMSYSTEM type = DEC);
	String( const double f, const int decimals = 2);
	~String();

	char operator[] ( const uint index);
	String operator= ( const String str);
	String operator+ ( const String str);
	String operator+= ( const String str);
	bool operator== ( const String str);
	bool operator> ( const String str);
	bool operator>= ( const String str);
	bool operator< ( const String str);
	bool operator<= ( const String str);
	bool operator!= ( const String str);

	char charAt( const uint index);
	int compareTo( const String str);
	String concat( const String str1, const String str2);
	const char* c_str();
	bool endsWith( const String str);
	bool equals( const String str);
	bool equalsIgnoreCase( const String str);
	void getBytes( uint8_t buf[], const uint len);
	int indexOf(  const String str);
	int indexOf( const char* str);
	int lastIndexOf( const String str, const uint from = 0);
	int lastIndexOf( const char* str, const uint from = 0);
	int length();
	void remove( const uint index, const uint count = 1);
	String replace(  const String str1, const String str2);
	void reserve( const int size);
	void setCharAt( const uint index, const char chr);
	bool startsWith( const String str);
	String substring( const uint from, const uint to = -1);
	void toCharArray( char buf[], const uint len);
	int toInt();
	double toFloat();
	void toLowerCase();
	void toUpperCase();
	void trim();

	static String toHex( const uint8_t *data, const uint size);
	static String toHexChar( const uint8_t *data, const uint size);
	static String fillout( const String str, const char c, const uint size);

protected:

	string S;
};

#endif // ARDUINOSTRING_H
