// file:   tcpserver.h
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.

// PROTOCOL
//
// 1. Client starts by sending an id-string
// 2. Server answers with "#START#" or,
//    when no id from client received, with "#END#" and quits
// 3. Data excange
// 4. Client or server ends by sending "#END#"
// 5. If Client was first, server echos "#END#"

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "ArduinoCore.h"
#include "ArduinoString.h"
#include "linkedlist.h"

class TcpServer
{
public:

	TcpServer();
	~TcpServer();

	bool connect( uint16_t port);
	bool connected();
	void close();

    void getClients( StringList& list);
    bool hasClient( String client);
	void stopClient( String client);

	String client();  // acts on first client in the list
	bool dataReady(); // acts on first client in the list
	bool dataReady( String client);
	bool read( String client, String& data);
	bool send( String client, String data);

    static String latestError();
    static StringList* latestErrors();

protected:

	int		m_socket;
};

#endif
