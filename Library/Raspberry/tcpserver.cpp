// file:   tcpserver.cpp
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
//    when no id from client received, send "#END#" and quit
// 3. Data exchange
// 4. Client or server ends by sending "#END#"

#include "tcpserver.h"
#include <signal.h>
#include <sys/ioctl.h>
namespace sckt {
#include <unistd.h>
#include <netdb.h>
}

const String START = "#START#";
const String END = "#END#";
bool g_end = false;
uint32_t g_timeout = 100;

LinkedList<TcpService*>	g_clients;

////////////////////
// ERROR HANDLING //
////////////////////

StringList g_error;

void error( String msg)
{
    g_error.add( msg);
    if ( g_error.size() > 10 )
        g_error.removeAt( 0);
}

String latestError()
{
    if ( g_error.size() > 0 )
        return g_error.at( g_error.size() - 1);
    return "";
}

///////////////////
// CLIENT THREAD //
///////////////////

bool readFromClient( int fd, String& data)
{
    char buffer[256];
    long tm = millis() + g_timeout;
    while ( tm > millis() ) {
        bzero( buffer, 256);
        int n = sckt::read( fd, buffer, 255);
        if ( n < 0 ) continue;
        data = buffer;
        return true;
    }
    error( "Routine: readFromClient.\nFailed to receive data from a client (timeout).\n");
    return false;
}

bool sendToClient( int fd, String data)
{
    int n = sckt::write( fd, data.c_str(), data.length());
    if ( n < 0 )
        error( "Routine: sendToClient.\nFailed to send data to a client.\n");
    return ( n >= 0 );
}

void* clientThread( void* socket)
{
    int lsocket = *((int*)socket);
    String data, sclient;
    int lclient;

    while ( !g_end ) {
	//
        // accept new client if available
	//
        struct sckt::sockaddr_in cli_addr;
        sckt::socklen_t clilen;
        clilen = sizeof( cli_addr);
        lclient = -1;
        lclient = sckt::accept( lsocket, (struct sckt::sockaddr*) &cli_addr, &clilen);

        if ( lclient >= 0 ) {
            // set lclient to non-blocking mode
            int on = 1;
            if ( ioctl( lclient, FIONBIO, (char*) &on) < 0 ) {
                sckt::close( lclient);
                error( "Routine: clientThread.\nFailed to set non-blocking mode.\n");
                continue;
            }

            // wait for client name
            if ( !readFromClient( lclient, sclient) ) {
                sendToClient( lclient, END);
                sckt::close( lclient);
                error( "Routine: clientThread.\nFailed to receive client id.\n");
                continue;
            }

	    // if client reconnects, first delete old connection
	    for ( int i = 0; i < g_clients.size(); i++ ) {
		TcpService* cli = g_clients.at( i);
		if ( cli->client() == sclient ) {
		    g_clients.removeAt( i);
		    delete cli;
		    break;
		}
	    }

	    // add client to the list
	    TcpService *cli = new TcpService( lclient);
	    if ( !cli ) {
		sckt::close( lclient);
		error( "Routine: clientThread.\nFailed to allocate memory for new client.\n");
		continue;
	    }
	    cli->setClient( sclient);
	    g_clients.add( cli);

            // send acknowledge
            if ( !sendToClient( lclient, START) )
                error( "Routine: clientThread.\nFailed to send acknowledge to client.\n");
        }

	//
        // handle incoming data and requested service termination
	//
        for ( int i = g_clients.size() - 1; i >= 0; i-- ) {
	    TcpService *cli = g_clients.at( i);
	    if ( cli->mustDelete() && !cli->onList() ) {
		g_clients.removeAt( i);
		delete cli;
	    }
	    else
            if ( !cli->read() ) {
		// service received close request
		Serial.println( String( "Service '") + cli->client() + "' stopped.");
		cli->markDelete();
	    }
        }
    }

    // close all clients
    for ( int i = g_clients.size() - 1; i >= 0; i-- ) {
	TcpService *cli = g_clients.at( i);
	cli->close();
        delete cli;
	delay( 10);
    }
    g_clients.removeAll();

    pthread_exit( 0);             
}

////////////////
// TCPSERVICE //
////////////////

TcpService::TcpService( int stream)
{
    m_stream = stream;
    m_status = 0;
    m_lists  = 0;
    m_delete = false;
}

void TcpService::setClient( String client)
{
    m_client = client;
}

void TcpService::setStatus( uint8_t status)
{
    m_status = status;
}

void TcpService::setInfo( String info)
{
    m_info = info;
}

String TcpService::client()
{
    return m_client;
}

uint8_t TcpService::status()
{
    return m_status;
}

String TcpService::info()
{
    return m_info;
}

void TcpService::send( String data)
{
    sendToClient( m_stream, data);
}

String TcpService::receive()
{
    if ( m_data.size() <= 0 ) return "";
    String data = m_data.at( 0);
    m_data.removeAt( 0);
    return data;
}

bool TcpService::read()
{
    String data;
    if ( readFromClient( m_stream, data) ) {
	if ( data == END ) {
	    sckt::close( m_stream);
	    return false;
	}
	else
	    m_data.add( data);
    }
    return true;
}

bool TcpService::dataReady()
{
    return (m_data.size() > 0);
}

void TcpService::close()
{
    sendToClient( m_stream, END);
    sckt::close( m_stream);
}

void TcpService::registerList()
{
    m_lists++;
}

void TcpService::releaseList()
{
    m_lists--;
    if ( m_lists < 0 ) m_lists = 0;
}

bool TcpService::onList()
{
    return (m_lists > 0);
}

void TcpService::markDelete()
{
    m_delete = true;
}

bool TcpService::mustDelete()
{
    return m_delete;
}

///////////////
// TCPSERVER //
///////////////

TcpServer::TcpServer()
{
    m_socket = -1;
}

TcpServer::~TcpServer()
{
    close();
}

bool TcpServer::connect( uint16_t port)
{
    // create server socket
    struct sckt::sockaddr_in serv_addr;
    m_socket = sckt::socket( AF_INET, sckt::SOCK_STREAM, 0);
    if ( m_socket < 0 ) {
        error( "Routine: connect.\nFailed to open socket.\n");
        return false;
    }

    // set to non-blocking connection
    int on = 1;
    if ( ioctl( m_socket, FIONBIO, (char*) &on) < 0 ) {
        sckt::close( m_socket);
	m_socket = -1;
        error( "Routine: connect.\nFailed to set non-blocking mode.\n");
        return false;
    }

    // bind socket to port
    bzero( (char*) &serv_addr, sizeof( serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = 0; // INADDR_ANY is defined as '(in_addr_t)0x00000000'
    serv_addr.sin_port = sckt::htons( port);
    if ( sckt::bind( m_socket, (struct sckt::sockaddr*) &serv_addr, sizeof( serv_addr)) < 0 ) {
        sckt::close( m_socket);
	m_socket = -1;
        error( "Routine: connect.\nFailed to bind socket to an address.");
        return false;
    }

    // start listening to max 10 clients
    sckt::listen( m_socket, 10);

    // start client communication thread
    pthread_t thread;
    g_end = false;
    pthread_create( &thread, 0, clientThread, (void*) &m_socket);
    pthread_detach( thread);

    return true;
}

bool TcpServer::isConnected()
{
    return (m_socket >= 0);
}

void TcpServer::close()
{
    g_end = true; // this causes the clientThread to stop all services
    while ( g_clients.size() ) ; // wait until all services stopped
    sckt::close( m_socket);
    m_socket = -1;
}

uint8_t	TcpServer::serviceCount()
{
    return g_clients.size();
}

TcpService* TcpServer::service( uint8_t index)
{
    if ( index < g_clients.size() )
	return g_clients.at( index);
    return NULL;
}

TcpService* TcpServer::service( String service)
{
    for ( int i = 0; i < g_clients.size(); i++ ) {
	if ( g_clients.at( i)->client() == service )
	    return g_clients.at( i);
    }
    return NULL;
}

//////////////////////////////////////////////
// NEVER DELETE SERVICES FROM A SERVICELIST //
// SERVICE DELETION IS AN AUTOMATIC PROCESS //
//////////////////////////////////////////////


bool TcpServer::exists( ServiceList& list, String service)
{
    for ( int i = 0; i < list.size(); i++ ) {
	TcpService* cli = list.at( i);
	if ( (cli->client() == service) && !cli->mustDelete() )
	    return true;
    }
    return false;
}

void TcpServer::remove( ServiceList& list, String service)
{
    for ( int i = list.size() - 1; i >= 0; i-- ) {
	TcpService* cli = list.at( i);
	if ( cli->client() == service ) {
	    list.removeAt( i);
	    cli->releaseList();
	}
    }
}

void TcpServer::allServices( ServiceList& list)
{
    clearServiceList( list);
    for ( int i = 0; i < g_clients.size(); i++ ) {
	TcpService* cli = list.at( i);
	if ( !cli->mustDelete() ) {
	    cli->registerList();
	    list.add( cli);
	}
    }
}

void TcpServer::servicesAppendStatus( ServiceList& list, uint8_t status)
{
    for ( int i = 0; i < g_clients.size(); i++ ) {
	TcpService* cli = g_clients.at( i);
	if ( (cli->status() == status) && !cli->mustDelete() ) {
	    cli->registerList();
	    list.add( cli);
	}
    }
}

void TcpServer::servicesAppendInfo( ServiceList& list, String info)
{
    for ( int i = 0; i < g_clients.size(); i++ ) {
	TcpService* cli = g_clients.at( i);
	if ( (cli->info() == info) && !cli->mustDelete() ) {
	    cli->registerList();
	    list.add( cli);
	}
    }
}

void TcpServer::servicesRemoveStatus( ServiceList& list, uint8_t status)
{
    for ( int i = list.size() - 1; i >= 0; i-- ) {
	TcpService* cli = list.at( i);
	if ( cli->status() == status ) {
	    list.removeAt( i);
	    cli->releaseList();
	}
    }
}

void TcpServer::servicesRemoveInfo( ServiceList& list, String info)
{
    for ( int i = list.size() - 1; i >= 0; i-- ) {
	TcpService* cli = list.at( i);
	if ( cli->info() == info ) {
	    list.removeAt( i);
	    cli->releaseList();
	}
    }
}

uint8_t TcpServer::servicesHaveStatus( ServiceList& list, uint8_t status)
{
    uint8_t cnt = 0;
    for ( int i = 0; i < list.size(); i++ ) {
	TcpService* cli = list.at( i);
	if ( (cli->status() == status) && !cli->mustDelete() )
	    cnt++;
    }
    return cnt;
}

uint8_t TcpServer::servicesHaveInfo( ServiceList& list, String info)
{
    uint8_t cnt = 0;
    for ( int i = 0; i < list.size(); i++ ) {
	TcpService* cli = list.at( i);
	if ( (cli->info() == info) && !cli->mustDelete() )
	    cnt++;
    }
    return cnt;
}

void TcpServer::servicesSetStatus( ServiceList& list, uint8_t status)
{
    for ( int i = 0; i < list.size(); i++ )
	list.at( i)->setStatus( status);
}

void TcpServer::servicesSetInfo( ServiceList& list, String info)
{
    for ( int i = 0; i < list.size(); i++ )
	list.at( i)->setInfo( info);
}

void TcpServer::servicesSend( ServiceList& list, String data)
{
    for ( int i = 0; i < list.size(); i++ ) {
	TcpService* cli = list.at( i);
	if ( !cli->mustDelete() )
	    cli->send( data);
    }
}

void TcpServer::clearServiceList( ServiceList& list)
{
    for ( int i = list.size() - 1; i >= 0; i-- )
	list.at( i)->releaseList();
    list.removeAll();
}

String TcpServer::latestError()
{
    if ( !g_error.size() ) return "No errors.";
    return g_error.at( 0);
}

StringList* TcpServer::latestErrors()
{
    return &g_error;
}
