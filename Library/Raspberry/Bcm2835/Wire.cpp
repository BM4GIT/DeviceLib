// Wire.cpp

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

#include "BcmCore.h"
#include "Wire.h"


namespace unistd {
	//All functions of unistd.h must be called like this: unistd::the_function()
    #include <unistd.h>
};


WirePi Wire = WirePi();

timeval start_program;


/******************
 * Public methods *
 ******************/

//Constructor
WirePi::WirePi(){
	REV = getBoardRev();
	if(map_peripheral(&gpio) == -1) {
		printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
	}
	
	memfd = -1;
	i2c_byte_wait_us = 0;
	
	// Open the master /dev/memory device
    if ((memfd = open("/dev/mem", O_RDWR | O_SYNC) ) < 0) 
    {
	fprintf(stderr, "bcm2835_init: Unable to open /dev/mem: %s\n",
		strerror(errno)) ;
	exit(1);
    }
	
	bcm2835_bsc01 = mapmem("bsc1", BLOCK_SIZE, memfd, BCM2835_BSC1_BASE2);
    if (bcm2835_bsc01 == MAP_FAILED) exit(1);
	
    // start timer
    gettimeofday(&start_program, NULL);
    
}

//Initiate the Wire library and join the I2C bus.
void WirePi::begin(){

	volatile uint32_t* paddr = bcm2835_bsc01 + BCM2835_BSC_DIV/4;

    // Set the I2C/BSC1 pins to the Alt 0 function to enable I2C access on them
    ch_gpio_fsel(RPI_V2_GPIO_P1_03, BCM2835_GPIO_FSEL_ALT0); // SDA
    ch_gpio_fsel(RPI_V2_GPIO_P1_05, BCM2835_GPIO_FSEL_ALT0); // SCL

    // Read the clock divider register
    uint16_t cdiv = ch_peri_read(paddr);
    // Calculate time for transmitting one byte
    // 1000000 = micros seconds in a second
    // 9 = Clocks per byte : 8 bits + ACK
    i2c_byte_wait_us = ((float)cdiv / BCM2835_CORE_CLK_HZ) * 1000000 * 9;
}

//Begin a transmission to the I2C slave device with the given address
void WirePi::beginTransmission(unsigned char address){
	// Set I2C Device Address
	volatile uint32_t* paddr = bcm2835_bsc01 + BCM2835_BSC_A/4;
	ch_peri_write(paddr, address);
}

//Writes data to the I2C.
void WirePi::write(char data){
	
	char i2cdata[1];
	i2cdata[0] = data;
	
	write(i2cdata,1);
	
}

//Writes data to the I2C.
uint8_t WirePi::write(const char * buf, uint32_t len){
	
	volatile uint32_t* dlen    = bcm2835_bsc01 + BCM2835_BSC_DLEN/4;
    volatile uint32_t* fifo    = bcm2835_bsc01 + BCM2835_BSC_FIFO/4;
    volatile uint32_t* status  = bcm2835_bsc01 + BCM2835_BSC_S/4;
    volatile uint32_t* control = bcm2835_bsc01 + BCM2835_BSC_C/4;

    uint32_t remaining = len;
    uint32_t i = 0;
    uint8_t reason = BCM2835_I2C_REASON_OK;

    // Clear FIFO
    ch_peri_set_bits(control, BCM2835_BSC_C_CLEAR_1 , BCM2835_BSC_C_CLEAR_1 );
    // Clear Status
	ch_peri_write_nb(status, BCM2835_BSC_S_CLKT | BCM2835_BSC_S_ERR | BCM2835_BSC_S_DONE);
	// Set Data Length
    ch_peri_write_nb(dlen, len);
    // pre populate FIFO with max buffer
    while( remaining && ( i < BCM2835_BSC_FIFO_SIZE ) )
    {
        ch_peri_write_nb(fifo, buf[i]);
        i++;
        remaining--;
    }
    
    // Enable device and start transfer
    ch_peri_write_nb(control, BCM2835_BSC_C_I2CEN | BCM2835_BSC_C_ST);
    
    // Transfer is over when BCM2835_BSC_S_DONE
    while(!(ch_peri_read_nb(status) & BCM2835_BSC_S_DONE ))
    {
        while ( remaining && (ch_peri_read_nb(status) & BCM2835_BSC_S_TXD ))
    	{
        	// Write to FIFO, no barrier
        	ch_peri_write_nb(fifo, buf[i]);
        	i++;
        	remaining--;
    	}
    }

    // Received a NACK
    if (ch_peri_read(status) & BCM2835_BSC_S_ERR)
    {
		reason = BCM2835_I2C_REASON_ERROR_NACK;
    }

    // Received Clock Stretch Timeout
    else if (ch_peri_read(status) & BCM2835_BSC_S_CLKT)
    {
		reason = BCM2835_I2C_REASON_ERROR_CLKT;
    }

    // Not all data is sent
    else if (remaining)
    {
		reason = BCM2835_I2C_REASON_ERROR_DATA;
    }

    ch_peri_set_bits(control, BCM2835_BSC_S_DONE , BCM2835_BSC_S_DONE);

    return reason;
}


void WirePi::endTransmission(){
	// Set all the I2C/BSC1 pins back to input
    ch_gpio_fsel(RPI_V2_GPIO_P1_03, BCM2835_GPIO_FSEL_INPT); // SDA
    ch_gpio_fsel(RPI_V2_GPIO_P1_05, BCM2835_GPIO_FSEL_INPT); // SCL
}

//Used by the master to request bytes from a slave device
void WirePi::requestFrom(unsigned char address,int quantity){
	// Set I2C Device Address
	volatile uint32_t* paddr = bcm2835_bsc01 + BCM2835_BSC_A/4;
	ch_peri_write(paddr, address);
	
	i2c_bytes_to_read = quantity;
}

//Reads a byte that was transmitted from a slave device to a master after a call to WirePi::requestFrom()
unsigned char WirePi::read(){
	char buf;
	i2c_bytes_to_read=1;
	read(&buf);
	return (unsigned char)buf;
}

uint8_t WirePi::read(char* buf){
    volatile uint32_t* dlen    = bcm2835_bsc01 + BCM2835_BSC_DLEN/4;
    volatile uint32_t* fifo    = bcm2835_bsc01 + BCM2835_BSC_FIFO/4;
    volatile uint32_t* status  = bcm2835_bsc01 + BCM2835_BSC_S/4;
    volatile uint32_t* control = bcm2835_bsc01 + BCM2835_BSC_C/4;

    uint32_t remaining = i2c_bytes_to_read;
    uint32_t i = 0;
    uint8_t reason = BCM2835_I2C_REASON_OK;

    // Clear FIFO
    ch_peri_set_bits(control, BCM2835_BSC_C_CLEAR_1 , BCM2835_BSC_C_CLEAR_1 );
    // Clear Status
	ch_peri_write_nb(status, BCM2835_BSC_S_CLKT | BCM2835_BSC_S_ERR | BCM2835_BSC_S_DONE);
	// Set Data Length
    ch_peri_write_nb(dlen, i2c_bytes_to_read);
    // Start read
    ch_peri_write_nb(control, BCM2835_BSC_C_I2CEN | BCM2835_BSC_C_ST | BCM2835_BSC_C_READ);
    
    // wait for transfer to complete
    while (!(ch_peri_read_nb(status) & BCM2835_BSC_S_DONE))
    {
        // we must empty the FIFO as it is populated and not use any delay
        while (ch_peri_read_nb(status) & BCM2835_BSC_S_RXD)
    	{
    		// Read from FIFO, no barrier
    		buf[i] = ch_peri_read_nb(fifo);
        	i++;
        	remaining--;
    	}
    }
    
    // transfer has finished - grab any remaining stuff in FIFO
    while (remaining && (ch_peri_read_nb(status) & BCM2835_BSC_S_RXD))
    {
        // Read from FIFO, no barrier
        buf[i] = ch_peri_read_nb(fifo);
        i++;
        remaining--;
    }
    
    // Received a NACK
    if (ch_peri_read(status) & BCM2835_BSC_S_ERR)
    {
		reason = BCM2835_I2C_REASON_ERROR_NACK;
    }

    // Received Clock Stretch Timeout
    else if (ch_peri_read(status) & BCM2835_BSC_S_CLKT)
    {
		reason = BCM2835_I2C_REASON_ERROR_CLKT;
    }

    // Not all data is received
    else if (remaining)
    {
		reason = BCM2835_I2C_REASON_ERROR_DATA;
    }

    ch_peri_set_bits(control, BCM2835_BSC_S_DONE , BCM2835_BSC_S_DONE);

    return reason;
}


// Read an number of bytes from I2C sending a repeated start after writing
// the required register. Only works if your device supports this mode
uint8_t WirePi::read_rs(char* regaddr, char* buf, uint32_t len){   
    volatile uint32_t* dlen    = bcm2835_bsc01 + BCM2835_BSC_DLEN/4;
    volatile uint32_t* fifo    = bcm2835_bsc01 + BCM2835_BSC_FIFO/4;
    volatile uint32_t* status  = bcm2835_bsc01 + BCM2835_BSC_S/4;
    volatile uint32_t* control = bcm2835_bsc01 + BCM2835_BSC_C/4;
    
	uint32_t remaining = len;
    uint32_t i = 0;
    uint8_t reason = BCM2835_I2C_REASON_OK;
    
    // Clear FIFO
    ch_peri_set_bits(control, BCM2835_BSC_C_CLEAR_1 , BCM2835_BSC_C_CLEAR_1 );
    // Clear Status
	ch_peri_write_nb(status, BCM2835_BSC_S_CLKT | BCM2835_BSC_S_ERR | BCM2835_BSC_S_DONE);
	// Set Data Length
    ch_peri_write_nb(dlen, 1);
    // Enable device and start transfer
    ch_peri_write_nb(control, BCM2835_BSC_C_I2CEN);
    ch_peri_write_nb(fifo, regaddr[0]);
    ch_peri_write_nb(control, BCM2835_BSC_C_I2CEN | BCM2835_BSC_C_ST);
    
    // poll for transfer has started
    while ( !( ch_peri_read_nb(status) & BCM2835_BSC_S_TA ) )
    {
        // Linux may cause us to miss entire transfer stage
        if(ch_peri_read(status) & BCM2835_BSC_S_DONE)
            break;
    }
    
    // Send a repeated start with read bit set in address
    ch_peri_write_nb(dlen, len);
    ch_peri_write_nb(control, BCM2835_BSC_C_I2CEN | BCM2835_BSC_C_ST  | BCM2835_BSC_C_READ );
    
    // Wait for write to complete and first byte back.	
    delayMicroseconds( (uint64_t) i2c_byte_wait_us * 3);
    
    // wait for transfer to complete
    while (!(ch_peri_read_nb(status) & BCM2835_BSC_S_DONE))
    {
        // we must empty the FIFO as it is populated and not use any delay
        while (remaining && ch_peri_read_nb(status) & BCM2835_BSC_S_RXD)
    	{
    		// Read from FIFO, no barrier
    		buf[i] = ch_peri_read_nb(fifo);
        	i++;
        	remaining--;
    	}
    }
    
    // transfer has finished - grab any remaining stuff in FIFO
    while (remaining && (ch_peri_read_nb(status) & BCM2835_BSC_S_RXD))
    {
        // Read from FIFO, no barrier
        buf[i] = ch_peri_read_nb(fifo);
        i++;
        remaining--;
    }
    
    // Received a NACK
    if (ch_peri_read(status) & BCM2835_BSC_S_ERR)
    {
		reason = BCM2835_I2C_REASON_ERROR_NACK;
    }

    // Received Clock Stretch Timeout
    else if (ch_peri_read(status) & BCM2835_BSC_S_CLKT)
    {
		reason = BCM2835_I2C_REASON_ERROR_CLKT;
    }

    // Not all data is sent
    else if (remaining)
    {
		reason = BCM2835_I2C_REASON_ERROR_DATA;
    }

    ch_peri_set_bits(control, BCM2835_BSC_S_DONE , BCM2835_BSC_S_DONE);

    return reason;
}


/*******************
 * Private methods *
 *******************/

// Exposes the physical address defined in the passed structure using mmap on /dev/mem
int WirePi::map_peripheral(struct bcm2835_peripheral *p)
{
   // Open /dev/mem
   if ((p->mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("Failed to open /dev/mem, try checking permissions.\n");
      return -1;
   }

   p->map = mmap(
      NULL,
      BLOCK_SIZE,
      PROT_READ|PROT_WRITE,
      MAP_SHARED,
      p->mem_fd,  // File descriptor to physical memory virtual file '/dev/mem'
      p->addr_p      // Address in physical map that we want this memory block to expose
   );

   if (p->map == MAP_FAILED) {
        perror("mmap");
        return -1;
   }

   p->addr = (volatile unsigned int *)p->map;

   return 0;
}

void WirePi::unmap_peripheral(struct bcm2835_peripheral *p) {

    munmap(p->map, BLOCK_SIZE);
    unistd::close(p->mem_fd);
}

void WirePi::wait_i2c_done() {
        //Wait till done, let's use a timeout just in case
        int timeout = 50;
        while((!((BSC0_S) & BSC_S_DONE)) && --timeout) {
            unistd::usleep(1000);
        }
        if(timeout == 0)
            printf("wait_i2c_done() timeout. Something went wrong.\n");
}
