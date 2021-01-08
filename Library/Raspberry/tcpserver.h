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

class TcpService
{
public:

	TcpService( int stream);

	void setClient( String client);
	void setStatus( uint8_t status);
	void setInfo( String info);

	String client();
	uint8_t status();
	String info();

	void send( String data);
	String receive();

	bool read(); // returns false when services receive close request
	bool dataReady();

	void close();

	void registerList();	// register service is in a list
	void releaseList();		// unregister service from a list
	bool onList();			// returns if service is registerd on some list

	void markDelete();		// mark service for deletion
	bool mustDelete();		// returns if service should be deleted

protected:

	int			m_stream;
	String		m_client;
	StringList	m_data;
	uint8_t		m_status;
	String		m_info;
	uint8_t		m_lists;
	bool		m_delete;
};

typedef LinkedList<TcpService*>	ServiceList;

class TcpServer
{
public:

	TcpServer();
	~TcpServer();

	// network connection
	bool connect( uint16_t port);
	bool isConnected();
	void close();

	// services
	uint8_t	serviceCount();
	TcpService* service( uint8_t index);
	TcpService* service( String service);

	//////////////////////////////////////////////////////////////////////
	// CALL clearServiceList WHEN A LIST IS NO LONGER IN USE            //
	// NEVER CALL add, removeAt OR removeAll DIRECTLY FOR A SERVICELIST //
	//////////////////////////////////////////////////////////////////////

	// report if a service exist in the specified list
	bool exists( ServiceList& list, String service);
	void remove( ServiceList& list, String service);

	// append all available service to the list
	void allServices( ServiceList& list);

	// remove all services that have the status or info from the specified list
	void servicesAppendStatus( ServiceList& list, uint8_t status);
	void servicesAppendInfo( ServiceList& list, String info);

	// remove all services that have the status or info from the specified list
	void servicesRemoveStatus( ServiceList& list, uint8_t status);
	void servicesRemoveInfo( ServiceList& list, String info);

	// return the number of services in the specified list having the status or info
	uint8_t servicesHaveStatus( ServiceList& list, uint8_t status);
	uint8_t servicesHaveInfo( ServiceList& list, String info);

	// set the status or info for all services in the specified list 
	void servicesSetStatus( ServiceList& list, uint8_t status);
	void servicesSetInfo( ServiceList& list, String info);

	// send data to all services in the specified list
	void servicesSend( ServiceList& list, String data);

	// safe deletion of a service list
	void clearServiceList( ServiceList& list);

	// error handling
    String latestError();
    StringList* latestErrors();

protected:

	int	m_socket;
};

#endif
