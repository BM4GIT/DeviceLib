// file:   tcpclient.h
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.

#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#ifdef RPI
#include "ArduinoCore.h"
#include "ArduinoString.h"
#else
#include <Arduino.h>
#endif

class TcpClient
{
public:

	TcpClient();

  bool wifi( String ssid, String password);

	bool connect( String host, uint16_t port, String id);
	bool isConnected();
	bool close();

	bool read( String& data);
	bool send( String data);

  String error();

protected:

  String  m_error;
	int		m_socket;
	bool	m_end;
};

#endif
