// SPI.h

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
*  Version BmLib, 2020 (For BmLib library on Raspberry)
*  The complete library has been split up to mirror the arduino configuration.
*  These are the components in the BmLib library:
*  > BcmCore		- all helper stuff
*  > ArduinoCore	- global routines like pinMode, digitalWrite, etc. and the Serial interface
*  > Wire			- the I2C interface
*  > SPI			- the SPI interface
*  Some minor changes have been made to meet the structure of the BmLib library.
*/

#ifndef SPI_H
#define SPI_H

class SPIPi{
public:
	SPIPi();
	void begin();
	void end();
	void setBitOrder(uint8_t order);
	void setClockDivider(uint16_t divider);
	void setDataMode(uint8_t mode);
	void chipSelect(uint8_t cs);
	void setChipSelectPolarity(uint8_t cs, uint8_t active);
	uint8_t transfer(uint8_t value);
	void transfernb(char* tbuf, char* rbuf, uint32_t len);
};

// SPI declaration
extern SPIPi SPI;

#endif // SPI_H
