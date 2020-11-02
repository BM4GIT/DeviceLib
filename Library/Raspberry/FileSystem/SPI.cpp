// file:    SPI.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.



#include "SPI.h"
#include "ArduinoCore.h"
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

spi SPI;

SPISettings::SPISettings() {
    m_clock = SPI_CLOCK_DIV2;
    m_speed = 1000000;
    m_first = LSBFIRST;
    m_mode = SPI_MODE0;
}

SPISettings::SPISettings( uint8_t speed, uint8_t lsb_msb, uint8_t spimode) {
    m_clock = 1; //### omrekenen van speed naar clock
    m_speed = speed;
    m_first = lsb_msb;
    m_mode = spimode;
}

spi::spi()
{
  lock=false;
  device = "/dev/spidev0.0";
  bits=8;
  delay=0;
  first = MSBFIRST;
  spitr.tx_buf=(uint64_t)tx;
  spitr.rx_buf=(uint64_t)rx;
  spitr.rx_nbits=0;
  spitr.tx_nbits=0;
  spitr.len=MAX_BUF_SIZE;
  spitr.delay_usecs=delay;
  spitr.speed_hz=1000000;
  spitr.bits_per_word=bits;
  spitr.pad=0;
  spitr.cs_change=0;
}

spi::~spi()
{
    endTransaction();
}


void spi::beginTransaction( SPISettings settings)
 {
    int ret;
    spiStatus.code=0;
    spiStatus.txt="";
    spitr.rx_nbits=0;
    spitr.rx_nbits=0;
    spitr.pad=0;
    first = settings.m_first;
    if ( fd ) close(fd); // if not yet done, end previous transaction first
    fd = open(device, O_RDWR);
    if (fd == -1) {
        spiStatus.code=-1;
        spiStatus.txt="SPI: can't open device";
        fd = 0;
    }
    ret = ioctl(fd, SPI_IOC_WR_MODE, &settings.m_mode);
    if (ret == -1){
        spiStatus.code=1;
        spiStatus.txt="SPI: can't set write mode";
    }
    ret = ioctl(fd, SPI_IOC_RD_MODE, &settings.m_mode);
    if (ret == -1){
        spiStatus.code=2;
        spiStatus.txt="SPI: can't set read mode";
    }
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1){
        spiStatus.code=3;
        spiStatus.txt="SPI: can't set write bits per word";
    }
    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1){
        spiStatus.code=4;
        spiStatus.txt="SPI: can't set read bits per word";
    }
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &settings.m_speed);
    if (ret == -1){
        spiStatus.code=5;
        spiStatus.txt="SPI: can't set write speed";
    }
    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &settings.m_speed);
    if (ret == -1){
        spiStatus.code=6;
        spiStatus.txt="SPI: can't set read speed";
    }
    spitr.delay_usecs=delay;
    spitr.speed_hz=settings.m_speed;
    spitr.bits_per_word=bits;
}

void spi::endTransaction()
{
    if ( fd ) close(fd);
    fd = 0;
}

uint8_t spi::transfer( uint8_t buffer)
{
    int ret;
    uint8_t in;
    spiStatus.code=0;
    spiStatus.txt="";
    if (lock){
      spiStatus.code=-2;
      return 0;
    }
    lock=true;
    spitr.tx_buf=(uint64_t)&buffer;
    spitr.rx_buf=(uint64_t)&in;
    spitr.len=1;
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &spitr);
    if (ret == -1){
        spiStatus.code=8;
        spiStatus.txt="SPI: can't send byte";
        in=0;
    }
    lock=false;
    return in;
}

uint16_t spi::transfer16( uint16_t buffer)
{
    int ret;
    uint16_t in;
    spiStatus.code=0;
    spiStatus.txt="";
    if (lock){
      spiStatus.code=-2;
      return 0;
    }
    lock=true;
    spitr.tx_buf=(uint64_t)&buffer;
    spitr.rx_buf=(uint64_t)&in;
    spitr.len=2;
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &spitr);
    if (ret == -1){
        spiStatus.code=8;
        spiStatus.txt="SPI: can't send byte";
        in=0;
    }
    lock=false;
    return in;
}

void spi::transfer( void* buffer, size_t len)
{
    uint8_t in[len];
    int ret;
    spiStatus.code=0;
    spiStatus.txt="";
    if (lock){
      spiStatus.code=-2;
      return;
    }
    lock=true;
    spitr.tx_buf=(uint64_t)buffer;
    spitr.rx_buf=(uint64_t)in;
    spitr.len=len;
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &spitr);
    if (ret == -1){
        spiStatus.code=7;
        spiStatus.txt="SPI: can't send buffer";
    }
    lock=false;
    for ( uint i = 0; i < len; i++ )
        ((uint8_t*)buffer)[i] = in[i];
}

