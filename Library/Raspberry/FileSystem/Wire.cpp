// file:    Wire.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#include "Wire.h"
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port
#include "ArduinoCore.h"

namespace unistd {
    //All functions of unistd.h must be called like this: unistd::the_function()
    #include <unistd.h>
};

wire Wire;

wire::wire()
{
  addr=8;
  status.code=0;
  status.txt = "";
  wridx = 0;
}

wire::~wire(){
  closeBus();
}

void wire::closeBus(){
  unistd::close(file_i2c);
}

int wire::begin(int adr){
  if ((file_i2c = open(filename.c_str(), O_RDWR)) < 0){
    status.code=1;
    status.txt="Failed to open the i2c bus.";
    return status.code;
  }
  if (adr){
    addr=adr;
    if (ioctl(file_i2c, I2C_SLAVE, addr) < 0){
      status.code=2;
      status.txt="Failed to acquire bus access as slave.";
      return status.code;
    }
  }
  return 0;
}

int wire::setAddr(){
  if (ioctl(file_i2c, I2C_SLAVE, addr) < 0){
    status.code=2;
    status.txt="Failed to acquire bus access as slave.";
    return status.code;
  }
  return 0;
}

int wire::requestFrom(uint8_t adr, int len)
{
  addr=adr;
  rdidx=0;
  if (setAddr()){
    return 0;
  }
  rdlen=unistd::read(file_i2c, rdbuff, len);//read() returns the number of bytes actually read, if it doesn't match then an error occurred (e.g. no response from the device)
  if (rdlen != len){
    status.code=3;
    status.txt="Failed to read from the i2c bus.";
  }
  return rdlen;
}

int wire::available(){
  if ((rdlen-rdidx) >= 0 ){
    return rdlen-rdidx;
  }else{
    return 0;
  }
}

uint8_t wire::read(){
  if (rdidx < sizeof(rdbuff)){
    return rdbuff[rdidx++];
  }else{
    return 0;
  }
}

void wire::beginTransmission(uint8_t adr)
{
  addr=adr;
  wridx=0;
  wrlen=0;
}

int wire::write(uint8_t dat, int len)
{
  if (wridx >= 40){
    return 0;
  }
  wrbuff[wridx++]=dat;
  return 1;
}

int wire::write(String dat, int len)
{
  int i;
  int cnt=0;
  if (wridx >= 40){
    return 0;
  }
  for (i=0; i<dat.length(); i++){
    if (wridx < 40){
      wrbuff[wridx++]=dat[i];
      cnt++;
    }
  }
  return cnt;
}

int wire::write(uint8_t * dat, int len)
{
  int i;
  int cnt=0;
  if (wridx >= 40){
    return 0;
  }
  for (i=0; i<len; i++){
    if (wridx < 40){
      wrbuff[wridx++]=dat[i];
      cnt++;
    }
  }
  return cnt;
}

int wire::endTransmission()
{
  int wrt;
  if (setAddr()){
    return 0;
  }
  wrt=unistd::write(file_i2c, wrbuff, wridx); //write() returns the number of bytes actually written
  if (wrt != wridx){	//if it doesn't match then an error occurred (e.g. no response from the device)
    status.code=4;
    status.txt="Failed to write to the i2c bus.";
  }
  wridx = 0;
  return wrt;
}

