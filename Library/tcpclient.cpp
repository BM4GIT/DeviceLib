// file:   tcpclient.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.

#include "tcpclient.h"
namespace sckt {
#include <unistd.h>
#include <netdb.h>
#include <sys/ioctl.h>
}

TcpClient::TcpClient()
{
	m_socket = -1;
}

bool TcpClient::connect( String host, uint16_t port, String id)
{
    struct sckt::sockaddr_in serv_addr;
    struct sckt::hostent *server;
    char buffer[256];

	// construct server information
	server = sckt::gethostbyname( host.c_str());
	if ( server == NULL ) {
        m_error = "Failed to find host.\n";
		return false;
    }

	// create socket
	m_socket = sckt::socket( AF_INET, sckt::SOCK_STREAM, 0);
	if (m_socket < 0) {
        m_error = "Failed to open socket.\n";
		return false;
    }

	// connect to server
	bzero( (char*) &serv_addr, sizeof( serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy( (char*) server->h_addr, (char*) &serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = sckt::htons( port);
	if ( sckt::connect( m_socket, (struct sckt::sockaddr*) &serv_addr, sizeof( serv_addr)) < 0 ) {
		sckt::close( m_socket);
		m_socket = -1;
        m_error = "Failed to connect to host.\n";
		return false;
    }


	// set to non-blocking connection
	int on = 1;
	if ( sckt::ioctl( m_socket, FIONBIO, (char*) &on) < 0 ) {
		sckt::close( m_socket);
		m_socket = -1;
        m_error = "Failed to set non-blocking mode.\n";
		return false;
    }

	// send client id
	if ( sckt::write( m_socket, id.c_str(), id.length()) < 0 ) {
		sckt::close( m_socket);
		m_socket = -1;
        m_error = "Failed to send client id.\n";
		return false;
    }

	// wait for acknowledgement
	m_end = false;
	while ( !m_end ) {
		bzero( buffer, 256);
		sckt::read( m_socket, buffer, 255);
		if ( !strcmp( buffer, "#START#") )
			break;
	}
    if ( m_end ) {
		sckt::close( m_socket);
		m_socket = -1;
        m_error = "Did not receive an acknowledgement from the server\n.";
		return false;
    }

	return true;
}

bool TcpClient::close()
{
    if ( m_socket < 0 ) {
        m_error = "Not connected to a server.\n";
        return false;
    }

	m_end = true;
	if ( sckt::write( m_socket, "#END#", 5) < 0 ) {
        m_error = "Failed to inform server of ending\n.";
		return false;
    }
	sckt::close( m_socket);
    return true;
}

bool TcpClient::connected()
{
	return (m_socket >= 0);
}

bool TcpClient::read( String& data)
{
    if ( m_socket < 0 ) {
        m_error = "Not connected to a server.\n";
        return false;
    }

    char buffer[256];
	bzero( buffer, 256);
	if ( sckt::read( m_socket, buffer, 255) < 0 ) {
        m_error = "Failed to read data.\n";
        return false;
    }

    if ( !strcmp( buffer, "#END#") ) {
        sckt::close( m_socket);
        m_socket = -1;
    }
    else
        data = buffer;
    return true;
}

bool TcpClient::send( String data)
{
    if ( m_socket < 0 ) {
        m_error = "Not connected to a server.\n";
        return false;
    }

	int n = sckt::write( m_socket, data.c_str(), data.length());
    if ( n < 0 ) {
        m_error = "Failed to send data.\n";
	}
	return (n >= 0);
}

String TcpClient::error()
{
    return m_error;
}
