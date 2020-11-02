// Wire.h

/*
*  Copyright (C) 2012 Libelium Comunicaciones Distribuidas S.L.
*  http://www.libelium.com
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Version 2.4 (For Raspberry Pi 2)
*  Author: Sergio Martinez, Ruben Martin
*
*  Version DeviceLib, 2020 (For DeviceLib library on Raspberry)
*  The complete library has been split up to mirror the arduino configuration.
*  These are the components in the DeviceLib library:
*  > BcmCore		- all helper stuff
*  > ArduinoCore	- global routines like pinMode, digitalWrite, etc. and the Serial interface
*  > Wire			- the I2C interface
*  > SPI			- the SPI interface
*  Some minor changes have been made to meet the structure of the DeviceLib library.
*/

#ifndef WIRE_H
#define WIRE_H

#include <sys/time.h>

// WirePi Class

class WirePi{
	private:
		int memfd;
		int i2c_byte_wait_us;
		int i2c_bytes_to_read;
		void dump_bsc_status();
		int map_peripheral(struct bcm2835_peripheral *p);
		void unmap_peripheral(struct bcm2835_peripheral *p);
		void wait_i2c_done();
	public:
		WirePi();
		void begin();
		void beginTransmission(unsigned char address);
		void write(char data);
		uint8_t write(const char * buf, uint32_t len);
		void endTransmission();
		void requestFrom(unsigned char address,int quantity);
		unsigned char read();
		uint8_t read(char* buf);
		uint8_t read_rs(char* regaddr, char* buf, uint32_t len);
};

// Wire declaration
extern WirePi Wire;

#endif // WIRE_H
