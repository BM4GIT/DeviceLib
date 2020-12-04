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
// 3. Data excange
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

///////////////
// MESSAGING //
///////////////

typedef struct
{
    String id;
    String data;
} message_t;

LinkedList<message_t*>	g_msg_in;
LinkedList<message_t*>	g_msg_out;

bool add_send_message( String id, String data)
{
    message_t *msg;
    try {
    msg = new message_t;
    }
    catch ( bad_alloc* e ) {
    error( String( "Client: ") + id + String( ", Message: ") + data + ".\nRoutine: add_send_message.\nFailed allocate memory for a message to send.\n");
    return false;
    }
    msg->id = id;
    msg->data = data;
    g_msg_out.add( msg);
    return true;
}

bool add_recv_message( String id, String data)
{
    message_t *msg;
    try {
    msg = new message_t;
    }
    catch ( bad_alloc* e ) {
    error( String( "Client: ") + id + String( ", Message: ") + data + ".\nRoutine: add_recv_message.\nFailed to allocate memory for a received message.\n");
    return false;
    }
    msg->id = id;
    msg->data = data;
    g_msg_in.add( msg);
    return true;
}

bool get_send_message( String id, String& data)
{
    for ( int i = 0; i < g_msg_out.size(); i++ )
        if ( g_msg_out.at( i)->id == id ) {
            data = g_msg_out.at( i)->data;
            delete g_msg_out.at( i);
            g_msg_out.removeAt( i);
            return true;
        }
    error( String( "Client: ") + id + ". Routine: get_send_message.\nNo more messages to send.\n");
    return false;
}

bool get_recv_message( String id, String& data)
{
    for ( int i = 0; i < g_msg_in.size(); i++ )
        if ( g_msg_in.at( i)->id == id ) {
            data = g_msg_in.at( i)->data;
            delete g_msg_in.at( i);
            g_msg_in.removeAt( i);
            return true;
        }
    error( String( "Client: ") + id + ". Routine: get_recv_message.\nNo more message received.\n");
    return false;
}

///////////////////
// CLIENT THREAD //
///////////////////

bool readFromClient( int fd, String& data, long timeout)
{
    char buffer[256];
    long tm = millis() + timeout;
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

typedef struct
{
    String id;
    int	   fd;
} client_t;

LinkedList<client_t*>	g_clients;

void deleteClient( String client)
{
    message_t* msg;
    client_t* cli;
    int i;
    for ( i = 0; i < g_msg_in.size(); i++ ) {
        msg = g_msg_in.at( i);
        if ( msg->id == client ) {
            delete msg;
            g_msg_in.removeAt( i);
        }
    }
    for ( i = 0; i < g_msg_out.size(); i++ ) {
        msg = g_msg_out.at( i);
        if ( msg->id == client ) {
            delete msg;
            g_msg_out.removeAt( i);
        }
    }
    for ( i = 0; i < g_clients.size(); i++ ) {
        cli = g_clients.at( i);
        if ( cli->id == client ) {
            delete cli;
            g_clients.removeAt( i);
        }
    }
}

void* clientThread( void* socket)
{
    int lsocket = *((int*)socket);
    String data, sclient;
    int lclient;

    while ( !g_end ) {

        // accept new client if available
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
            if ( !readFromClient( lclient, sclient, g_timeout) ) {
                sendToClient( lclient, END);
                sckt::close( lclient);
                error( "Routine: clientThread.\nFailed to receive client id.\n");
                continue;
            }

	    // if client reconnects, first delete old connection
	    int i;
	    for ( i = 0; i < g_clients.size(); i++ )
		if ( g_clients.at( i)->id == sclient ) {
		    deleteClient( sclient);
		    break;
		}

	    // add client to the list
	    client_t *cli;
	    try {
	    cli = new client_t;
	    }
	    catch ( bad_alloc* e ) {
	    sckt::close( lclient);
	    error( "Routine: clientThread.\nFailed to allocate memory for new client.\n");
	    continue;
	    }
	    cli->id = sclient;
	    cli->fd = lclient;
	    g_clients.add( cli);

            // send acknowledge
            if ( !sendToClient( lclient, START) )
                error( "Routine: clientThread.\nFailed to send acknowledge to client.\n");
        }

        // handle data exchange
        int ix = 0;
        while ( ix < g_clients.size() ) {

            // send pending outgoing data
            if ( get_send_message( g_clients.at( ix)->id, data) ) {
               if ( data == END ) {
                    sckt::close( g_clients.at( ix)->fd);
                    deleteClient( g_clients.at( ix)->id);
                    break;
                }
                else {
                    if ( !sendToClient( g_clients.at( ix)->fd, data) )
                        error( "Routine: clientThread.\nFailed to send data to client.\n");
                }

            }
            else
                error( "Routine: clientThread.\nFailed to retrieve data from send-list.\n");

            // receive pending incoming data
            if ( readFromClient( g_clients.at( ix)->fd, data, g_timeout) ) {
                if ( data == END ) {
                    sckt::close( g_clients.at( ix)->fd);
                    deleteClient( g_clients.at( ix)->id);
                    break;
                }
                else {
                    if ( !add_recv_message( g_clients.at( ix)->id, data) )
                        error( "Routine: clientThread.\nFailed to add data to received-list.\n");
                }
            }
            else
                error( "Routine: clientThread.\nFailed to receive data from client.\n");

            ix++;
        }
    }

    // close all clients
    for ( int i = g_clients.size() - 1; i >= 0; i-- ) {
        sendToClient( g_clients.at( i)->fd, END);
        sckt::close( g_clients.at( i)->fd);
        deleteClient( g_clients.at( i)->id);
    }
    // although all messages should have been deleted, just in case ...
    for ( int i = 0; i < g_msg_in.size(); i++ ) {
        delete g_msg_in.at( i);
        g_msg_in.removeAt( i);
    }
    for ( int i = 0; i < g_msg_out.size(); i++ ) {
        delete g_msg_out.at( i);
        g_msg_out.removeAt( i);
    }

    pthread_exit( 0);             
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

bool TcpServer::connected()
{
    return (m_socket >= 0);
}

void TcpServer::close()
{
    g_end = true;
    while ( g_clients.size() ) ; // wait until all client threads stopped
    sckt::close( m_socket);
    m_socket = -1;
}

void TcpServer::getClients( StringList& list)
{
    for ( int i = 0; i < g_clients.size(); i++ ) {
	int j;
        for ( j = 0; j < list.size(); j++ ) {
            if ( list.at( j) == g_clients.at( i)->id )
                break;
        }
        if ( j >= list.size() )
            list.add( g_clients.at( i)->id);
    }
}

bool TcpServer::hasClient( String client)
{
    for ( int i = 0; i < g_clients.size(); i++ )
	if ( client == g_clients.at( i)->id )
	    return true;
    return false;
}

void TcpServer::stopClient( String client)
{
    for ( int i = 0; i < g_clients.size(); i++ ) {
        if ( g_clients.at( i)->id == client ) {
            if ( !add_send_message( client, END) )
                error( "Routine: stopClient.\nFailed to send end-command to client.\n");
            break;
        }
    }
}

bool TcpServer::dataReady()
{
    if ( g_msg_in.size() )
	    return true;
    return false;
}

bool TcpServer::dataReady( String client)
{
    for ( int i = 0; i < g_msg_in.size(); i++ )
	if ( g_msg_in.at( i)->id == client )
	    return true;
    return false;
}

String TcpServer::client()
{
    if ( g_msg_in.size() )
	return g_msg_in.at( 0)->id;
    return "";
}

bool TcpServer::read( String client, String& data)
{
    if ( !get_recv_message( client, data) ) {
        error( "Routine: read.\nFailed to retrieve data from the receive-list.\n");
        return false;
    }
    return true;
}

bool TcpServer::send( String client, String data)
{
    if ( !add_send_message( client, data) ) {
        error( "Routine: send.\nFailed to add data to the send-list.\n");
        return false;
    }
    return true;
}

String TcpServer::latestError()
{
    return latestError();
}

StringList* TcpServer::latestErrors()
{
    return &g_error;
}
