// file:    json.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of BmLib. BmLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: BmLib is distributed without any warranty.

#include "json.h"

Json::Json()
{
}

Json::Json( const Json& json)
{
	m_obj = json.m_obj;
}

Json Json::add( const Json value)
{
  if ( m_obj.length() ) m_obj += ", ";
  m_obj += value.m_obj;
  return *this;
}

Json Json::add( const String value)
{
  if ( m_obj.length() ) m_obj += ", ";
  m_obj += String( "\"") + value + "\"";
  return *this;
}

Json Json::add( const char* value)
{
  String val( value);
  add( val);
  return *this;
}

Json Json::add( const double value, int decimals)
{
  if ( m_obj.length() ) m_obj += ", ";
  m_obj += String( value, decimals);
  return *this;
}

Json Json::add( const String key, const Json value)
{
  if ( !key.length() ) return *this;
  if ( m_obj.length() ) m_obj += ", ";
  m_obj += String( "\"") + key + "\":";
  if ( value.m_obj.length() && (value.m_obj[0] == '{' || value.m_obj[0] == '[') )
    m_obj += value.m_obj;
  else
    m_obj += String( "\"") + value.m_obj + "\"";
  return *this;
}

Json Json::add( const String key, const String value)
{
  if ( !key.length() ) return *this;
  if ( m_obj.length() ) m_obj += ", ";
  m_obj += String( "\"") + key + "\":";
  if ( value.length() && (value[0] == '{' || value[0] == '[') )
    m_obj += value;
  else
    m_obj += String( "\"") + value + "\"";
  return *this;
}

Json Json::add( const String key, const char* value)
{
  String val( value);
  add( key, val);
  return *this;
}

Json Json::add( const String key, const double value, int decimals)
{
  if ( !key.length() ) return *this;
  if ( m_obj.length() ) m_obj += ", ";
  m_obj += String( "\"") + key + "\":";
  m_obj += String( value, decimals);
  return *this;
}

Json Json::object( bool keep)
{
  Json json;
	json.m_obj = String( "{") + m_obj + "}";
  if ( !keep ) m_obj = "";
	return json;
}

Json Json::array( bool keep)
{
  Json json;
  json.m_obj = String( "[") + m_obj + "]";
  if ( !keep ) m_obj = "";
  return json;
}

Json Json::operator= ( const Json json)
{
	m_obj = json.m_obj;
	return json;
}

String Json::toString()
{
  return m_obj;
}

void Json::clear()
{
  m_obj = "";
}

uint16_t Json::count( const String& key)
{
    String json = m_obj;
    int jlen = json.length();
    int klen = key.length();
    int j = 0;
    int k = 0;
    uint16_t cnt = 0;

    while ( j < jlen ) {
        if ( json[j] == key[0] ) {
            for ( k = 0; k < klen; k++ ) {
                if ( j == jlen ) break;
                if ( json[j] != key[k] ) break;
                j++;
            }
            if ( k >= klen &&           // found the same text as in 'key'
                 j + 1 < jlen &&
                 json[j] == '\"' &&     // ending with a quote is required
                 json[j+1] == ':' ) {   // a key is followed by a colon
                // indeed found an occurence of the key
                j++;
                cnt++;
            }
        }
        j++;
    }

    return cnt;
}

void Json::get( String& value, const String key, const uint8_t occurence)
{
    String json = m_obj;
    int jlen = json.length();
    int klen = key.length();
    int j = 0;
    int k = 0;
    int occ = occurence;

    value = "";

    while ( j < jlen ) {
        if ( json[j] == key[0] ) {
            for ( k = 0; k < klen; k++ ) {
                if ( j == jlen ) break;
                if ( json[j] != key[k] ) break;
                j++;
            }
            if ( k >= klen &&           // found the same text as in 'key'
                 j + 1 < jlen &&
                 json[j] == '\"' &&     // ending with a quote is required
                 json[j+1] == ':' ) {   // a key is followed by a colon
                // indeed found an occurence of the key
                j++;
                occ--;
                if ( !occ ) break;
            }
        }
        j++;
    }
    if ( j >= jlen ) return; // THE TARGET IS NOT FOUND

    // find the value
    j++;
    if ( j < jlen && json[j] == '\"' )
        j++;
    value = "";
    while ( j < jlen && json[j] != '\"' && json[j] != ',' && json[j] != '}' && json[j] != ']' ) {
        value += json[j];
        j++;
    }
    value.trim();
    if ( value.length() && (value[0] == '\"') )
        value = value.substring( 1, value.length() - 1); // assume a closing quote too
}

void Json::get( double& value, const String key, const uint8_t occurence)
{
  String val;
  get( val, key, occurence);
  val.toUpperCase();
  value = val.toFloat();
}

void Json::get( long& value, const String key, const uint8_t occurence)
{
  String val;
  get( val, key, occurence);
  value = val.toFloat();
}
