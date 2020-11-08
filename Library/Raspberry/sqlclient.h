// file:   sqlclient.h
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#ifndef SQLCLIENT_H
#define SQLCLIENT_H

#ifndef RPI
#error NOTE: You cannot use 'sqlclient.h' on Arduino
#else

#include "Arduino.h"

class SqlClient
{
public:

	SqlClient();
	~SqlClient();

	bool connect( const String host, const String database, const String user, const String password);
	void close();
	String error();

	// 'query' sends a query string to the server.
	// You can build the query via the helper routines.
	// The query result is reported via 'nextRow'.
	// Each time 'nextRow' gets called the columns of the next row
	// in a set are returned as a StringList.
	// When the end of the rows is reached, an empty list is returned.
	// The field names of the result are reported via 'columnHeaders'.
	bool       query( const String querystr);
	void nextRow( StringList& row);
	void columnHeaders( StringList& header);

    // helper routines

	// There are two versions of 'filloutQuery':
	// - the first takes a StringList as value list
	// - the second takes a variable argument list (type String) for it.
	// The routine takes a query string that contains '@@n' tokens.
	// These tokens are replaced by the consequetive values in the list.
	// Thus in the query "UPDATE bikes WITH brand = @@b WHERE brand = @@1;"
	// @@b will be replaced by the first value in the list and @@1 by the second.
	// If a token is used more than once it will reference to the same value.
	// In the query "SELECT @@z FROM @@4 WHERE @@4id = @@z;" both @@z tokens are
	// replaced by the first value (since @@z occurs first in it) and both @@4
	// tokens are replaced by the second value (since @@4 occurs second).
	// NB! The number of different '@@n' tokens MUST match the number of values!
    static String filloutQuery( const String querystr, StringList &values);
    static String filloutQuery( const String querystr, ...);

protected:

};

#endif // RPI

#endif
