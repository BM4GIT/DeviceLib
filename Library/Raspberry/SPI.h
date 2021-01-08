// file:    SPI.h
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.


#ifndef SPI_H
#define SPI_H

#include "ArduinoString.h"
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define MAX_BUF_SIZE 35

#define SPI_CLOCK_DIV2  0

#define LSBFIRST    0
#define MSBFIRST    1

#define SPI_MODE0   0x00
#define SPI_MODE1   0x04
#define SPI_MODE2   0x08
#define SPI_MODE3   0x0C

typedef struct td_spi_tr {
    uint64_t tx_buf;
    uint64_t rx_buf;
    uint32_t len;
    uint32_t speed_hz;
    uint16_t delay_usecs;
    uint8_t bits_per_word;
    uint8_t	cs_change;
    uint8_t	tx_nbits;
    uint8_t	rx_nbits;
    uint16_t pad;
}td_spi_tr;

class SPISettings
{
public:
    SPISettings();
    SPISettings( uint32_t speed, uint8_t lsb_msb, uint8_t spimode);

    uint8_t     m_mode;
    uint8_t     m_first;
    uint8_t	    m_clock;
    uint32_t    m_speed;
};

class spi
{
public:
    explicit spi();
    ~spi();
    void beginTransaction( SPISettings settings);
    void endTransaction();
    uint8_t transfer( uint8_t buffer);
    uint16_t transfer16( uint16_t buffer);
    void transfer( void* buffer, size_t len);

    uint8_t bits;
    uint32_t speed;
    uint16_t delay;
    struct spiStatus {
        int code;
        String txt;
    }spiStatus;

private:
    uint8_t tx[MAX_BUF_SIZE];
    uint8_t rx[MAX_BUF_SIZE];
    td_spi_tr spitr;
    const char *device;
    int fd;
    bool lock;
    uint8_t first;
};

extern spi SPI;

#endif // SPI_H
