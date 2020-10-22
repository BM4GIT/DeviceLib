// file:   Wire.h
// Copyright 2020 D.E.Repolev
//
// This file is part of BmLib. BmLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: BmLib is distributed without any warranty.


#ifndef WIRE_H
#define WIRE_H

#include "ArduinoString.h"
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port

class wire
{
public:
  explicit wire();
  ~wire();
  int begin(int adr=0);
  void closeBus();
  int requestFrom(uint8_t adr, int len);
  int available();
  uint8_t read();
  void beginTransmission(uint8_t adr);
  int write(uint8_t dat, int len=0);
  int write(String dat, int len=0);
  int write(uint8_t * dat, int len);
  int endTransmission();
  struct status {
      int code;
      String txt;
  }status;
  uint8_t rdlen;
  uint8_t wrlen;
  uint8_t rdidx;
  uint8_t wridx;
  uint8_t rdbuff[40] = {0};
  uint8_t wrbuff[40] = {0};
  uint8_t addr;

private:
  int setAddr();
  int file_i2c;
  String filename = "/dev/i2c-1";
};

extern wire Wire;

#endif // WIRE_H
