// file:    ArduinoString.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of BmLib. BmLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: BmLib is distributed without any warranty.


#include "ArduinoString.h"

String::String()
{
}

String::String( const String &str)
{
	S = str.S;
}

String::String( const char chr)
{
	S = chr;
}

String::String( const char* str)
{
	S = str;
}

String::String( const string &str)
{
	S = str;
}

String::String( const int val, const NUMSYSTEM type)
{
	uint i = val;
	uint mask = 0xF;
	uint shift = 4;
	switch ( type ) {

		case BIN :	S = "";
					do {
						S.insert( 0, 1, ((i & 1) ? '1' : '0'));
						i = i >> 1;
					} while ( i );
					break;

		case OCT :	mask = 0x7;
					shift = 3;
		case HEX :	{
					uint8_t c;
					S = "";
					do {
						c = i & mask;
						if ( c < 10 ) c += '0';
						else c += ('A' - 10);
						S.insert( 0, 1, c);
						i = i >> shift;
					} while ( i );
					break;
					}

		default :	{
					char s[20];
					sprintf( s, "%d", i); S = s;
					break;
					}
											   
	}
}

String::String( const double f, const int decimals)
{
	char s[20];
	if ( decimals >= 0 ) {
		char fmt[10];
		sprintf( fmt, "%%0.%df", decimals);
		sprintf( s, fmt, f);
	}
	else
		sprintf( s, "%f", f);
	S = s;
}

String::~String()
{
}

char String::operator[] ( const uint index)
{
	return S[index];
}

String String::operator= ( const String str)
{
	S = str.S;
	return String( S);
}

String String::operator+ ( const String str)
{
	return S + str.S;
}

String String::operator+= ( const String str)
{
	S += str.S;
	return String( S);
}

bool String::operator== ( const String str)
{
	return S == str.S;
}

bool String::operator> ( const String str)
{
	return S > str.S;
}

bool String::operator>= ( const String str)
{
	return S >= str.S;
}

bool String::operator< ( const String str)
{
	return S < str.S;
}

bool String::operator<= ( const String str)
{
	return S <= str.S;
}

bool String::operator!= ( const String str)
{
	return S != str.S;
}

char String::charAt( const uint index)
{
	return S.at( index);
}

int String::compareTo( const String str)
{
	for ( uint i = 0; i < S.length() && i < str.S.length(); i++ )
		if ( S.at( i) != str.S.at( i) ) {
			if ( S.at( i) < str.S.at( i) ) return -i;
			return i;
		}
	return 0;
}

String String::concat( const String str1, const String str2)
{
	return str1.S + str2.S;
}

const char* String::c_str()
{
	return S.c_str();
}

bool String::endsWith( const String str)
{
	uint len = str.S.length();
	uint pos = S.length() - len;
	if ( pos > S.length() ) return false;
	return S.substr( pos, len) == str.S;
}

bool String::equals( const String str)
{
	return S == str.S;
}

bool String::equalsIgnoreCase( const String str)
{
	String s1 = str;
	String s2( S);
	s1.toLowerCase();
	s2.toLowerCase();
	return s1.S == s2.S;
}

void String::getBytes( uint8_t buf[], const uint len)
{
	uint l = len;
	if ( S.length() < l )
		l = S.length();
	S.copy( (char*) buf, l);
}

int String::indexOf( const String str)
{
//    return S.find_first_of( str.S); // seems not to work with \n\r characters
    int i, j;
    for ( i = 0; i < (int) S.length() - (int) str.S.length() + 1; i++ ) {
	for ( j = 0; j < (int) str.S.length(); j++ )
	    if ( S[i+j] != str.S[j] ) break;
	if ( j == (int) str.S.length() )
	    return i;
    }
    return -1;
}

int String::indexOf( const char* str)
{
    return indexOf( String( str));
}

int String::lastIndexOf( const String str, const uint from)
{
//    return S.find_last_of( str.S); // seems not to work with \n\r characters

    int i;
    uint j;
    for ( i = S.length() - str.S.length(); i >= (int) from; i-- ) {
	for ( j = 0; j < str.S.length(); j++ )
	    if ( S[i+j] != str.S[j] ) break;
	if ( j == str.S.length() )
	    return i;
    }
    return -1;
}

int String::lastIndexOf( const char* str, const uint from)
{
    return lastIndexOf( String( str), from);
}

int String::length()
{
//    return S.length(); // seems not to work with \n\r characters
   return strlen( S.c_str());
}

void String::remove( const uint index, const uint count)
{
	S.erase( index, count);
}

String String::replace( const String str1, const String str2)
{
	string s = S;
	int len = str1.S.length();
	int pos = s.find( str1.S);
	while ( pos >= 0 ) {
		s.replace( pos, len, str2.S);
		pos = s.find( str1.S);
	}
	return s;
}

void String::reserve( const int size)
{
	S.reserve( size);
}

void String::setCharAt( const uint index, const char chr)
{
	S.replace( index, 1, &chr);
}

bool String::startsWith( const String str)
{
	return S.find_first_of( str.S) == 0;
}

String String::substring( const uint from, const uint to)
{
	if ( to >= 0 )
		return S.substr( from, to - from);
	else
		return S.substr( from, S.length() - from);
}

void String::toCharArray( char buf[], const uint len)
{
	uint l = len;
	if ( S.length() < l )
		l = S.length();
	S.copy( buf, l);
}

int String::toInt()
{
	return stoi( S);
}

double String::toFloat()
{
	return stod( S);
}

void String::toLowerCase()
{
	for ( uint i = 0; i < S.length(); i++ )
		if ( S[i] >= 'A' && S[i] <= 'Z' ) {
			char c = S[i] - 'A' + 'a';
			S.replace( i, 1, &c);
		}
}

void String::toUpperCase()
{
	for ( uint i = 0; i < S.length(); i++ )
		if ( S[i] >= 'a' && S[i] <= 'z' ) {
			char c = S[i] - 'a' + 'A';
			S.replace( i, 1, &c);
		}
}

void String::trim()
{
	uint from, to;
	for ( from = 0; from < S.length(); from++ )
		if ( S[from] != ' ' ) break;
	for ( to = S.length() - 1; to >= 0; to-- )
		if ( S[from] != ' ' ) break;
	S = S.substr( from, to - from + 1);
}


String String::toHex( const uint8_t *data, const uint size)
{
	String str;
	uint32_t szPos;
	for ( szPos=0; szPos < size; szPos++ )
	{
		str += String( data[szPos] < 10 ? "0x0" : "0x");
		str += String( data[szPos], HEX);
		if ( (size > 1) && (szPos != size - 1) )
			str += " ";
	}
	return str;
}

String String::toHexChar( const uint8_t *data, const uint size)
{
	String str = toHex( data, size);
	uint32_t szPos;

	str += " ";
	for ( szPos=0; szPos < size; szPos++ )
	{
		if ( data[szPos] <= 0x1F )
			str += ".";
		else
			str += String( (char) data[szPos]);
	}
	return str;
}

String String::fillout( const String str, const char c, const uint size)
{
	String s = str;
	for ( uint i = s.length(); i < size; i++ )
		s += c;
	return s;
}
