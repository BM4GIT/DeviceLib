// SPI.cpp

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

#include "BcmCore.h"
#include "SPI.h"

SPIPi SPI = SPIPi();

void *spi0 = MAP_FAILED;
static  uint8_t *spi0Mem = NULL;

/******************
 * Public methods *
 ******************/

 SPIPi::SPIPi(){
	 
	REV = getBoardRev();

    uint8_t *mapaddr;

    if ((spi0Mem = (uint8_t*)malloc(BLOCK_SIZE + (PAGESIZE - 1))) == NULL){
        fprintf(stderr, "bcm2835_init: spi0Mem malloc failed: %s\n", strerror(errno));
        exit(1);
    }
    
    mapaddr = spi0Mem;
    if (((uint32_t)mapaddr % PAGESIZE) != 0)
        mapaddr += PAGESIZE - ((uint32_t)mapaddr % PAGESIZE) ;
    
    spi0 = (uint32_t *)mmap(mapaddr, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FIXED, gpio.mem_fd, BCM2835_SPI0_BASE2) ;
    
    if ((int32_t)spi0 < 0){
        fprintf(stderr, "bcm2835_init: mmap failed (spi0): %s\n", strerror(errno)) ;
        exit(1);
    }
 }

void SPIPi::begin(){
    // Set the SPI0 pins to the Alt 0 function to enable SPI0 access on them
    ch_gpio_fsel(7, BCM2835_GPIO_FSEL_ALT0); // CE1
    ch_gpio_fsel(8, BCM2835_GPIO_FSEL_ALT0); // CE0
    ch_gpio_fsel(9, BCM2835_GPIO_FSEL_ALT0); // MISO
    ch_gpio_fsel(10, BCM2835_GPIO_FSEL_ALT0); // MOSI
    ch_gpio_fsel(11, BCM2835_GPIO_FSEL_ALT0); // CLK
    
    // Set the SPI CS register to the some sensible defaults
    volatile uint32_t* paddr = (volatile uint32_t*)spi0 + BCM2835_SPI0_CS/4;
    ch_peri_write(paddr, 0); // All 0s
    
    // Clear TX and RX fifos
    ch_peri_write_nb(paddr, BCM2835_SPI0_CS_CLEAR);
}

void SPIPi::end(){  
    // Set all the SPI0 pins back to input
    ch_gpio_fsel(7, BCM2835_GPIO_FSEL_INPT); // CE1
    ch_gpio_fsel(8, BCM2835_GPIO_FSEL_INPT); // CE0
    ch_gpio_fsel(9, BCM2835_GPIO_FSEL_INPT); // MISO
    ch_gpio_fsel(10, BCM2835_GPIO_FSEL_INPT); // MOSI
    ch_gpio_fsel(11, BCM2835_GPIO_FSEL_INPT); // CLK
}

void SPIPi::setBitOrder(uint8_t order){
    // BCM2835_SPI_BIT_ORDER_MSBFIRST is the only one suported by SPI0
}

// defaults to 0, which means a divider of 65536.
// The divisor must be a power of 2. Odd numbers
// rounded down. The maximum SPI clock rate is
// of the APB clock
void SPIPi::setClockDivider(uint16_t divider){
    volatile uint32_t* paddr = (volatile uint32_t*)spi0 + BCM2835_SPI0_CLK/4;
    ch_peri_write(paddr, divider);
}

void SPIPi::setDataMode(uint8_t mode){
    volatile uint32_t* paddr = (volatile uint32_t*)spi0 + BCM2835_SPI0_CS/4;
    // Mask in the CPO and CPHA bits of CS
    ch_peri_set_bits(paddr, mode << 2, BCM2835_SPI0_CS_CPOL | BCM2835_SPI0_CS_CPHA);
}

// Writes (and reads) a single byte to SPI
uint8_t SPIPi::transfer(uint8_t value){
    volatile uint32_t* paddr = (volatile uint32_t*)spi0 + BCM2835_SPI0_CS/4;
    volatile uint32_t* fifo = (volatile uint32_t*)spi0 + BCM2835_SPI0_FIFO/4;

    ch_peri_set_bits(paddr, BCM2835_SPI0_CS_CLEAR, BCM2835_SPI0_CS_CLEAR);

    ch_peri_set_bits(paddr, BCM2835_SPI0_CS_TA, BCM2835_SPI0_CS_TA);

    while (!(ch_peri_read(paddr) & BCM2835_SPI0_CS_TXD))
    delayMicroseconds( (uint64_t) 10);

    ch_peri_write_nb(fifo, value);

    while (!(ch_peri_read_nb(paddr) & BCM2835_SPI0_CS_DONE))
    delayMicroseconds( (uint64_t) 10);

    uint32_t ret = ch_peri_read_nb(fifo);

    ch_peri_set_bits(paddr, 0, BCM2835_SPI0_CS_TA);

    return ret;
}

// Writes (and reads) a number of bytes to SPI
void SPIPi::transfernb(char* tbuf, char* rbuf, uint32_t len){
    volatile uint32_t* paddr = (volatile uint32_t*)spi0 + BCM2835_SPI0_CS/4;
    volatile uint32_t* fifo = (volatile uint32_t*)spi0 + BCM2835_SPI0_FIFO/4;

    // This is Polled transfer as per section 10.6.1
    // BUG ALERT: what happens if we get interupted in this section, and someone else
    // accesses a different peripheral? 

    // Clear TX and RX fifos
    ch_peri_set_bits(paddr, BCM2835_SPI0_CS_CLEAR, BCM2835_SPI0_CS_CLEAR);

    // Set TA = 1
    ch_peri_set_bits(paddr, BCM2835_SPI0_CS_TA, BCM2835_SPI0_CS_TA);

    uint32_t i;
    for (i = 0; i < len; i++)
    {
    // Maybe wait for TXD
    while (!(ch_peri_read(paddr) & BCM2835_SPI0_CS_TXD))
        delayMicroseconds( (uint64_t) 10);

    // Write to FIFO, no barrier
    ch_peri_write_nb(fifo, tbuf[i]);

    // Wait for RXD
    while (!(ch_peri_read(paddr) & BCM2835_SPI0_CS_RXD))
        delayMicroseconds( (uint64_t) 10);

    // then read the data byte
    rbuf[i] = ch_peri_read_nb(fifo);
    }
    // Wait for DONE to be set
    while (!(ch_peri_read_nb(paddr) & BCM2835_SPI0_CS_DONE))
    delayMicroseconds( (uint64_t) 10);

    // Set TA = 0, and also set the barrier
    ch_peri_set_bits(paddr, 0, BCM2835_SPI0_CS_TA);
}

void SPIPi::chipSelect(uint8_t cs){
    volatile uint32_t* paddr = (volatile uint32_t*)spi0 + BCM2835_SPI0_CS/4;
    // Mask in the CS bits of CS
    ch_peri_set_bits(paddr, cs, BCM2835_SPI0_CS_CS);
}

void SPIPi::setChipSelectPolarity(uint8_t cs, uint8_t active){
    volatile uint32_t* paddr = (volatile uint32_t*)spi0 + BCM2835_SPI0_CS/4;
    uint8_t shift = 21 + cs;
    // Mask in the appropriate CSPOLn bit
    ch_peri_set_bits(paddr, active << shift, 1 << shift);
}
