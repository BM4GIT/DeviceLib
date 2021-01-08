// file:    json.h
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#ifndef JSON_H
#define JSON_H

#include "ArduinoString.h"

class Json
{
public:

	Json();
	Json( const Json& json);

  // The next routines add  <value>  to m_obj (comma separated)
  // String values are placed between double quotes
  Json add( const Json value);
  Json add( const String value);
  Json add( const char* value); // Arduino IDE will choose boolean for literal texts
  Json add( const double value, int decimals = 2);

  // The next routines add  "<key>":<value>  to m_obj (comma separated)
  // String values are placed between double quotes
  Json add( const String key, const Json value);
  Json add( const String key, const String value);
  Json add( const String key, const char* value); // Arduino IDE will choose boolean for literal texts
  Json add( const String key, const double value, int decimals = 2);

  // Create an object or array of the current json contents
  // The current contents is cleared on request
  // Thereby first create a list and then make it an object or array
  // E.g:           j.add( 3.14); j.add( "Value"); j.add( "Key", j.object());
  // Result:        "Key":[3.14, "Value"]
  Json object( bool keep = false);	// returns '{m_obj}'
  Json array( bool keep = false);	// returns '[m_obj]'

  // Assign this json object
  Json operator= ( const Json json);
  String toString();

  // Clear this json object
  void clear();

  // Use the next routines to query a value by means of its key
  // Call 'count' to know number of occurrences of the key in the object
  // Call 'get' to retrieve the n'th occurence of the key

  uint16_t count( const String& key);

  void get( String& value, const String key, const uint8_t occurence = 1);
  void get( double& value, const String key, const uint8_t occurence = 1);
  void get( long& value, const String key, const uint8_t occurence = 1);

protected:

  String m_obj;
};

#endif
