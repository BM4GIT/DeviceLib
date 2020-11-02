// BcmCore.cpp

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
*  Author: Sergio, 2020 Martinez, Ruben Martin
*
*  Version DeviceLib (For DeviceLib library on Raspberry)
*  The complete library has been split up to mirror the arduino configuration.
*  These are the components in the DeviceLib library:
*  > BcmCore		- all helper stuff
*  > ArduinoCore	- global routines like pinMode, digitalWrite, etc. and the Serial interface
*  > Wire			- the I2C interface
*  > SPI			- the SPI interface
*  Some minor changes have been made to meet the structure of the DeviceLib library.
*/

#include <string>
#include <cstring>
#include <cerrno>
#include <sys/ioctl.h>
#include "BcmCore.h"

int REV = 0;

struct bcm2835_peripheral gpio = {GPIO_BASE2};
struct bcm2835_peripheral bsc_rev1 = {IOBASE + 0X205000};
struct bcm2835_peripheral bsc_rev2 = {IOBASE + 0X804000};
struct bcm2835_peripheral bsc0;
volatile uint32_t *bcm2835_bsc01;

int getBoardRev(){
	
	FILE *cpu_info;
	char line [120];
	char *c,finalChar;
	
	if (REV != 0) return REV;
	
	if ((cpu_info = fopen("/proc/cpuinfo","r"))==NULL){
		fprintf(stderr,"Unable to open /proc/cpuinfo. Cannot determine board reivision.\n");
		exit(1);
	}
	
	while (fgets (line,120,cpu_info) != NULL){
		if(strncmp(line,"Revision",8) == 0) break;
	}
	
	fclose(cpu_info);
	
	if (line == NULL){
		fprintf (stderr, "Unable to determine board revision from /proc/cpuinfo.\n") ;
		exit(1);
	}
	
	for (c = line ; *c ; ++c)
    if (isdigit (*c))
      break ;

	if (!isdigit (*c)){
		fprintf (stderr, "Unable to determine board revision from /proc/cpuinfo\n") ;
		fprintf (stderr, "  (Info not found in: %s\n", line) ;
		exit(1);
	}
	
	finalChar = c [strlen (c) - 2] ;
	
	if ((finalChar == '2') || (finalChar == '3')){
		bsc0 = bsc_rev1;
		return 1;
	}else{
		bsc0 = bsc_rev2;
		return 2;
	}
}

int raspberryPinNumber(int bcmPin /*int arduinoPin*/){
	/*
	switch(arduinoPin){
		case 1: return 14; break;
		case 2: return 18; break;
		case 3: return 23; break;
		case 4: return 24; break;
		case 5: return 25; break;
		case 6: return  4; break;
		case 7: return 17; break;
		case 8: if(REV == 1){return 21;}else{return 27;} break;
		case 9: return 22; break;
		case 10: return 8; break;
		case 11: return 10; break;
		case 12: return 9; break;
		case 13: return 11; break;
	}*/
	return bcmPin;
}

uint32_t* mapmem(const char *msg, size_t size, int fd, off_t off)
{
    uint32_t *map = (uint32_t *)mmap(NULL, size, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, off);
    if (MAP_FAILED == map)
	fprintf(stderr, "bcm2835_init: %s mmap failed: %s\n", msg, strerror(errno));
    return map;
}

// safe read from peripheral
uint32_t ch_peri_read(volatile uint32_t* paddr){
    uint32_t ret = *paddr;
    ret = *paddr;
    return ret;
    
}

// read from peripheral without the read barrier
uint32_t ch_peri_read_nb(volatile uint32_t* paddr){
    return *paddr;
}

// safe write to peripheral
void ch_peri_write(volatile uint32_t* paddr, uint32_t value){
    *paddr = value;
    *paddr = value;
}

// write to peripheral without the write barrier
void ch_peri_write_nb(volatile uint32_t* paddr, uint32_t value){
    *paddr = value;
}

// Set/clear only the bits in value covered by the mask
void ch_peri_set_bits(volatile uint32_t* paddr, uint32_t value, uint32_t mask){
    uint32_t v = ch_peri_read(paddr);
    v = (v & ~mask) | (value & mask);
    ch_peri_write(paddr, v);
}

void ch_gpio_fsel(uint8_t pin, uint8_t mode){
    // Function selects are 10 pins per 32 bit word, 3 bits per pin
    volatile uint32_t* paddr = (volatile uint32_t*)gpio.map + BCM2835_GPFSEL0/4 + (pin/10);
    uint8_t   shift = (pin % 10) * 3;
    uint32_t  mask = BCM2835_GPIO_FSEL_MASK << shift;
    uint32_t  value = mode << shift;
    ch_peri_set_bits(paddr, value, mask);
}
