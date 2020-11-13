// file:    ArduinoCore.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.

/*
*  The following copyright notes only concern:
*  > Serial interface
*  > attachInterrupt / detachInterrupt
*  > analogRead / analogWrite
*  > the pwm and threading helper-routines
* 
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
*  2020: Some minor changes have been made to meet the structure of the DeviceLib library.
*/


#include <thread>
#include <termio.h>
#include <fcntl.h>
#include "ArduinoCore.h"
#include <sys/mman.h>
#include <poll.h>

namespace unistd {
    //All functions of unistd.h must be called like this: unistd::the_function()
    #include <unistd.h>
};

gpio g_gpio;
hirestime g_start;

//////////////////////
// HELPER FUNCTIONS //
//////////////////////

hirestime currentTime()
{
    return chrono::high_resolution_clock::now();
}

void localTime( tm &lt, uint32_t &msec)
{
    hirestime ht = currentTime();
    time_t tt = chrono::system_clock::to_time_t( ht);
    hirestime dt = chrono::system_clock::from_time_t( tt);
    msec = diffMillis( ht, dt);
    memcpy( &lt, localtime( &tt), sizeof( tm));
}

ulong diffMillis( hirestime stop, hirestime start)
{
    return chrono::duration_cast<chrono::milliseconds>( stop - start).count();
}

ulong diffMicros( hirestime stop, hirestime start)
{
    return chrono::duration_cast<chrono::microseconds>( stop - start).count();
}

#ifndef GTK
struct pwmgoStruct {
	void (*pwmworker)();
	thread pwmthread;
	int  pin;
	int  high;
	int  low;
	bool go;
};

#define pwmgoMax 5 // number of pwmXLoop routines
pwmgoStruct pwmgo[pwmgoMax];

void pwm0loop()
{
    printf( "Started 'pwm0loop'.\n");
    while ( pwmgo[0].go ) {
	digitalWrite( pwmgo[0].pin, HIGH);
	delayMicroseconds( pwmgo[0].high);
	digitalWrite( pwmgo[0].pin, LOW);
	delayMicroseconds( pwmgo[0].low);
    }
    printf( "Stopped 'pwm0loop'.\n");
}

void pwm1loop()
{
    printf( "Started 'pwm1loop'.\n");
    while ( pwmgo[1].go ) {
	digitalWrite( pwmgo[1].pin, HIGH);
	delayMicroseconds( pwmgo[1].high);
	digitalWrite( pwmgo[1].pin, LOW);
	delayMicroseconds( pwmgo[1].low);
    }
    printf( "Stopped 'pwm1loop'.\n");
}

void pwm2loop()
{
    printf( "Started 'pwm2loop'.\n");
    while ( pwmgo[2].go ) {
	digitalWrite( pwmgo[2].pin, HIGH);
	delayMicroseconds( pwmgo[2].high);
	digitalWrite( pwmgo[2].pin, LOW);
	delayMicroseconds( pwmgo[2].low);
    }
    printf( "Stopped 'pwm2loop'.\n");
}

void pwm3loop()
{
    printf( "Started 'pwm3loop'.\n");
    while ( pwmgo[3].go ) {
	digitalWrite( pwmgo[3].pin, HIGH);
	delayMicroseconds( pwmgo[3].high);
	digitalWrite( pwmgo[3].pin, LOW);
	delayMicroseconds( pwmgo[3].low);
    }
    printf( "Stopped 'pwm3loop'.\n");
}

void pwm4loop()
{
    printf( "Started 'pwm4loop'.\n");
    while ( pwmgo[4].go ) {
	digitalWrite( pwmgo[4].pin, HIGH);
	delayMicroseconds( pwmgo[4].high);
	digitalWrite( pwmgo[4].pin, LOW);
	delayMicroseconds( pwmgo[4].low);
    }
    printf( "Stopped 'pwm4loop'.\n");
}
#endif

struct bcm2835_peripheral{
    unsigned long addr_p;
    int mem_fd;
    void *map;
    volatile unsigned int *addr;
};

struct ThreadArg{
    void (*func)();
    int pin;
};

pthread_t idThread2;
pthread_t idThread3;
pthread_t idThread4;
pthread_t idThread5;
pthread_t idThread6;
pthread_t idThread7;
pthread_t idThread8;
pthread_t idThread9;
pthread_t idThread10;
pthread_t idThread11;
pthread_t idThread12;
pthread_t idThread13;

#define IOBASE   0x3f000000
struct bcm2835_peripheral bsc_rev1 = {IOBASE + 0X205000};
struct bcm2835_peripheral bsc_rev2 = {IOBASE + 0X804000};
struct bcm2835_peripheral bsc0;

int getBoardRev()
{
	FILE *cpu_info;
	char line [120];
	char *c,finalChar;
	
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

pthread_t *getThreadIdFromPin(int pin){
	switch(pin){
		case 2: return &idThread2; break;
		case 3: return &idThread3; break;
		case 4: return &idThread4; break;
		case 5: return &idThread5; break;
		case 6: return &idThread6; break;
		case 7: return &idThread7; break;
		case 8: return &idThread8; break;
		case 9: return &idThread9; break;
		case 10: return &idThread10; break;
		case 11: return &idThread11; break;
		case 12: return &idThread12; break;
		case 13: return &idThread13; break;
	}
    return 0; // should never come here
}

/* This is the function that will be running in a thread if
 * attachInterrupt() is called */
void * threadFunction(void *args){
	ThreadArg *arguments = (ThreadArg *)args;
	int pin = arguments->pin;
	
	int GPIO_FN_MAXLEN = 32;
	int RDBUF_LEN = 5;
	
	char fn[GPIO_FN_MAXLEN];
	int fd,ret;
	struct pollfd pfd;
	char rdbuf [RDBUF_LEN];
	
	memset(rdbuf, 0x00, RDBUF_LEN);
	memset(fn,0x00,GPIO_FN_MAXLEN);
	
	snprintf(fn, GPIO_FN_MAXLEN-1, "/sys/class/gpio/gpio%d/value",pin);
	fd=open(fn, O_RDONLY);
	if(fd<0){
		perror(fn);
		exit(1);
	}
	pfd.fd=fd;
	pfd.events=POLLPRI;
	
	ret=unistd::read(fd,rdbuf,RDBUF_LEN-1);
	if(ret<0){
		perror("Error reading interrupt file\n");
		exit(1);
	}
	
	while(1){
		memset(rdbuf, 0x00, RDBUF_LEN);
		unistd::lseek(fd, 0, SEEK_SET);
		ret=poll(&pfd, 1, -1);
		if(ret<0){
			perror("Error waiting for interrupt\n");
			unistd::close(fd);
			exit(1);
		}
		if(ret==0){
			printf("Timeout\n");
			continue;
		}
		ret=unistd::read(fd,rdbuf,RDBUF_LEN-1);
		if(ret<0){
			perror("Error reading interrupt file\n");
			exit(1);
		}
		//Interrupt. We call user function.
		arguments->func();
	}
}

//////////////////
// ARDUINO CORE //
//////////////////

void delay( uint32_t msec)
{
    hirestime ht = currentTime();
    while ( diffMillis( currentTime(), ht) < msec );
}

void delayMicroseconds( uint32_t micr)
{
    hirestime ht = currentTime();
    while ( diffMicros( currentTime(), ht) < micr ) ;
}

long millis()
{
    hirestime now = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::milliseconds>( now - g_start).count();
}

long micros()
{
    hirestime now = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::microseconds>( now - g_start).count();
}

void pinMode( uint8_t pin, uint8_t mode)
{
  g_gpio.openGpio( pin);
  if ( !g_gpio.pinMode( pin, mode) )
  if ( mode == OUTPUT )
    g_gpio.digitalWrite( pin, 0);
}

void digitalWrite( uint8_t pin, uint8_t data)
{
  g_gpio.digitalWrite( pin, data);
}

uint8_t digitalRead( uint8_t pin)
{
  uint8_t val = g_gpio.digitalRead( pin);
  return val;
}

#ifndef GTK
void analogWrite( uint8_t pin, uint16_t level)
{
    if ( level < 0 ) level = 0;
    if ( level > 1023 ) level = 1023;
    int high = level;
    int low = 1023 - level;

    // if pin already in use
    for ( int i = 0; i < pwmgoMax; i++ )
	if ( pwmgo[i].pin == pin ) {
	    if ( !level ) {
		// stop thread
		pwmgo[i].go = false;
		pwmgo[i].pwmthread.join();
		pwmgo[i].high = -1;
		pwmgo[i].low = -1;
		pwmgo[i].pin = -1;
	    }
	    else {
		// renew thread
		pwmgo[i].high = high;
		pwmgo[i].low = low;
	    }
	    return;
	}

    // find a free thread
    for ( int i = 0; i < pwmgoMax; i++ )
	if ( pwmgo[i].pin < 0 ) {
	    // start thread
	    pwmgo[i].pin = pin;
	    pwmgo[i].high = high;
	    pwmgo[i].low = low;
	    pwmgo[i].go = true;
	    pwmgo[i].pwmthread = thread( pwmgo[i].pwmworker);
	    break;
	}
}
#endif

void attachInterrupt( uint8_t p,void (*f)(), uint m){
	int GPIOPin = p;
	pthread_t *threadId = getThreadIdFromPin(p);
	struct ThreadArg *threadArgs = (ThreadArg *)malloc(sizeof(ThreadArg));
	threadArgs->func = f;
	threadArgs->pin = GPIOPin;
	
	//Export pin for interrupt
	FILE *fp = fopen("/sys/class/gpio/export","w");
	if (fp == NULL){
		fprintf(stderr,"Unable to export pin %d for interrupt\n",p);
		exit(1);
	}else{
		fprintf(fp,"%d",GPIOPin); 
	}
	fclose(fp);
	
	//The system to create the file /sys/class/gpio/gpio<GPIO number>
	//So we wait a bit
	delay(1L);
	
	char * interruptFile = NULL;
	asprintf(&interruptFile, "/sys/class/gpio/gpio%d/edge",GPIOPin);
	
	//Set detection condition
	fp = fopen(interruptFile,"w");
	if (fp == NULL){
		fprintf(stderr,"Unable to set detection type on pin %d\n",p);
		exit(1);
	}else{
		switch(m){
			case RISING: fprintf(fp,"rising");break;
			case FALLING: fprintf(fp,"falling");break;
			default: fprintf(fp,"both");break;
		}
		
	}
	fclose(fp);
	
	if(*threadId == 0){
		//Create a thread passing the pin and function
		pthread_create (threadId, NULL, threadFunction, (void *)threadArgs);
	}else{
		//First cancel the existing thread for that pin
		pthread_cancel(*threadId);
		//Create a thread passing the pin, function and mode
		pthread_create (threadId, NULL, threadFunction, (void *)threadArgs);
	}
	
}

void detachInterrupt(uint8_t p){
	int GPIOPin = p;
	
	FILE *fp = fopen("/sys/class/gpio/unexport","w");
	if (fp == NULL){
		fprintf(stderr,"Unable to unexport pin %d for interrupt\n",p);
		exit(1);
	}else{
		fprintf(fp,"%d",GPIOPin); 
	}
	fclose(fp);
	
	pthread_t *threadId = getThreadIdFromPin(p);
	pthread_cancel(*threadId);
}


//////////////////////
// BIT MANIPULATION //
//////////////////////

void bitWrite( uint8_t &data, uint8_t bit, uint8_t value)
{
	uint8_t b = (1 << bit);
	if ( value )
		data |= b;
	else
		data &= (b xor 0xFF);
}

void bitSet( uint8_t &data, uint8_t bit)
{
	uint8_t b = (1 << bit);
	data |= b;
}

void bitClear( uint8_t &data, uint8_t bit)
{
	uint8_t b = (1 << bit);
	data &= (b xor 0xFF);
}

uint8_t bitRead( uint8_t data, uint8_t bit, uint8_t value)
{
	uint8_t b = data & (1 << bit);
	return (b ? 1 : 0);
}

uint32_t bit( uint8_t bit)
{
	return 1 << bit;
}

uint8_t lowByte( uint16_t byte)
{
	return (uint8_t) byte & 0x00FF;
}

uint8_t highByte( uint16_t byte)
{
	return (uint8_t) (byte & 0xFF00) >> 8;
}

//////////////////
// CLASS SERIAL //
//////////////////

SerialRPi::SerialRPi( bool isconsole)
{
    m_console = isconsole;
    m_enabled = false;
    m_timeout = 1000;
}

uint SerialRPi::available()
{
    uint avail = 0;
    if ( m_console ) {
	struct termios original;
	tcgetattr( STDIN_FILENO, &original);
	struct termios term;
	memcpy( &term, &original, sizeof( term));
	term.c_lflag &= ~ICANON;
	tcsetattr( STDIN_FILENO, TCSANOW, &term);
	ioctl( STDIN_FILENO, FIONREAD, &avail);
	tcsetattr( STDIN_FILENO, TCSANOW, &original);
    }
    else
    if ( ioctl( m_uart, FIONREAD, &avail) < 0)
	return 0;
    return avail;
}

uint SerialRPi::availableForWrite()
{
    return 0;
}

void SerialRPi::begin( const ulong baud, const uint config)
{
    if ( !m_console ) {

	// old style: "/dev/ttyAMA0"
	if ( (m_uart = open( "/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK)) == -1 )
	    return;

	printf( "UART settings:\n");
	printf( "  Baud rate = %d\n  ", (uint) baud);

	ulong speed;
	switch ( baud ) {
	    case 50:	speed = B50; break;
	    case 75:	speed = B75; break;
	    case 110:	speed = B110; break;
	    case 134:	speed = B134; break;
	    case 150:	speed = B150; break;
	    case 200:	speed = B200; break;
	    case 300:	speed = B300; break;
	    case 600:	speed = B600; break;
	    case 1200:	speed = B1200; break;
	    case 1800:	speed = B1800; break;
	    case 2400:	speed = B2400; break;
	    case 9600:	speed = B9600; break;
	    case 19200:	speed = B19200; break;
	    case 38400:	speed = B38400; break;
	    case 57600:	speed = B57600; break;
	    case 115200:speed = B115200; break;
	    case 230400:speed = B230400; break;
	    default:    speed = baud; // perhaps a none regular baud rate
	}

	uint parity   = config >> 3;		// 0=N, 1=E, 2=O
	uint stopbits = ((config >> 2) & 1);	// 0=1, 1=2
	uint databits = (config & 3) + 5;	// 5 ~ 8
	uint datasize = (config & 3) << 4;	// 5=0, 6=16, 7=32, 8=48

	struct termios options;
	tcgetattr( m_uart, &options);

	cfmakeraw( &options);
	cfsetispeed( &options, speed);
	cfsetospeed( &options, speed);

	options.c_cflag |= (CLOCAL | CREAD);

	if ( parity ) {
	    options.c_cflag |= PARENB;
	    if ( parity == 2 ) {
		options.c_cflag |= PARODD;
		printf( "Parity = Odd (Flags = 0x%02x), ", PARENB | PARODD);
	    }
	    else {
		options.c_cflag &= ~PARODD;
		printf( "Parity = Even (Flags = 0x%02x), ", PARENB);
	    }
	}
	else {
	    options.c_cflag &= ~PARENB;
	    printf( "No parity (Flags = 0x00), ");
	}

	options.c_cflag &= ~CSTOPB;
	options.c_cflag |= stopbits;
	printf( "Stopbits = %d (Flags = 0x%02x), ", stopbits + 1, stopbits);

	options.c_cflag &= ~CSIZE;
	options.c_cflag |= datasize;
	printf( "Databits = %d (Flags = 0x%02x)\n\n", databits, datasize);

	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag &= ~OPOST;

	tcflush( m_uart, TCIFLUSH);
	tcsetattr( m_uart, TCSANOW, &options);

	uint status;
	ioctl( m_uart, TIOCMGET, &status);
	status |= TIOCM_DTR;
	status |= TIOCM_RTS;
	ioctl( m_uart, TIOCMSET, &status);

	delay( 10);
    }
    m_enabled = true;
}

void SerialRPi::end()
{
    m_enabled = false;
    if ( m_console )
	unistd::close( m_uart);
}

bool SerialRPi::find( const String search)
{
    return findUntil( search);
}

bool SerialRPi::findUntil( const String search, const String stop)
{
    String stp= stop;
    String str;
    char chr = 0;
    hirestime ht = currentTime();
    while ( chr != '\n' ) {
	if ( diffMillis( currentTime(), ht) > m_timeout )
	    return false;
	if ( available() ) {
	    if ( m_console )
		chr = getchar();
	    else
		unistd::read( m_uart, &chr, 1);
	    str += chr;
	    if ( str.indexOf( search) ) break;
	    if ( stp.length() && (str.indexOf( stp) >= 0) ) break;
	}
    }
    return (chr != '\n');
}

void SerialRPi::flush()
{
    char chr;
    while( available() )
	if ( m_console )
	    chr = getchar();
	else
	    unistd::read( m_uart, &chr, 1);
}

double SerialRPi::parseFloat()
{
    String str;
    char chr = '0';
    hirestime ht = currentTime();
    while ( (chr < '0' || chr > '9') && (chr != '-' && chr != '.') ) {
	if ( diffMillis( currentTime(), ht) > m_timeout )
	    return 0;
	if ( available() ) {
	    if ( m_console )
		chr = getchar();
	    else
		unistd::read( m_uart, &chr, 1);
	}
    }
    while ( (chr >= '0' && chr <= '9') || (chr == '-' || chr == '.') ) {
	str += chr;
	if ( diffMillis( currentTime(), ht) > m_timeout )
	    return 0;
	if ( available() ) {
	    if ( m_console )
		chr = getchar();
	    else
		unistd::read( m_uart, &chr, 1);
	}
    }
    return str.toFloat();
}

long SerialRPi::parseInt()
{
    String str;
    char chr = '0';
    hirestime ht = currentTime();
    while ( (chr < '0' || chr > '9') && (chr != '-') ) {
	if ( diffMillis( currentTime(), ht) > m_timeout )
	    return 0;
	if ( available() ) {
	    if ( m_console )
		chr = getchar();
	    else
		unistd::read( m_uart, &chr, 1);
	}
    }
    while ( (chr >= '0' && chr <= '9') || (chr == '-') ) {
	str += chr;
	if ( diffMillis( currentTime(), ht) > m_timeout )
	    return 0;
	if ( available() ) {
	    if ( m_console )
		chr = getchar();
	    else
		unistd::read( m_uart, &chr, 1);
	}
    }
    return str.toInt();
}

int8_t SerialRPi::peek()
{
    if ( !available() )
	return -1;
    char c = 0;
    if ( !m_console ) {
	FILE * f = fdopen( m_uart,"r+");
	c = getc( f);
	ungetc( c, f);
    }
    return c;
}

uint SerialRPi::print( const int i, const NUMSYSTEM type)
{
    return print( String( i, type));
}

uint SerialRPi::print( const double f, const int decimals)
{
    return print( String( f, decimals));
}

uint SerialRPi::print( const String s)
{
    String str = s;
    return unistd::write( m_console ? STDOUT_FILENO : m_uart, str.c_str(), str.length());
}

uint SerialRPi::println( const int i, const NUMSYSTEM type)
{
    return print( String( i, type) + "\r\n");
}

uint SerialRPi::println( const double f, const int decimals)
{
    return print( String( f, decimals) + "\r\n");
}

uint SerialRPi::println( const String s)
{
    return print( String( s) + "\r\n");
}

int8_t SerialRPi::read()
{
    if ( !available() )
	return 0;
    char chr;
    if ( m_console )
	chr = getchar();
    else
	if ( !unistd::read( m_uart, &chr, 1) )
	    return 0;
    return chr;
}

uint8_t SerialRPi::readB( const char stop, uint8_t data[], const uint8_t len, bool dostop)
{
    memset( data, 0, len);
    char chr;
    uint8_t cnt = 0;
    hirestime ht = currentTime();
    do {
	if ( diffMillis( currentTime(), ht) > m_timeout )
	    break;
	if ( available() ) {
	    if ( m_console )
		chr = getchar();
	    else
		unistd::read( m_uart, &chr, 1);
	    if ( dostop && (chr == stop) )
		break;
	    data[cnt] = chr;
	    cnt++;
	}
    } while ( cnt < len );
    return cnt;
}

uint8_t SerialRPi::readBytes( uint8_t data[], const uint8_t len)
{
    return readB( 0, data, len, false);
}

uint8_t SerialRPi::readBytesUntil( const char stop, uint8_t data[], const uint8_t len)
{
    return readB( stop, data, len, true);
}

String SerialRPi::readS( const char stop, bool dostop)
{
    String str;
    char chr;
    hirestime ht = currentTime();
    do {
	if ( diffMillis( currentTime(), ht) > m_timeout )
	    break;
	if ( available() ) {
	    if ( m_console )
		chr = getchar();
	    else
		unistd::read( m_uart, &chr, 1);
	    if ( dostop && (chr == stop) )
		break;
	    str += chr;
	}
    }  while ( true );
    return str;
}

String SerialRPi::readString()
{
    if ( m_console ) {
	char inp[255];
	scanf( "%s", inp);
	return String( inp);
    }
    else
	return readS( 0, false);
}

String SerialRPi::readStringUntil( const char stop)
{
    return readS( stop, true);
}

void SerialRPi::setTimeout( ulong millis)
{
    m_timeout = millis;
}

uint SerialRPi::write( const uint8_t data)
{
    return unistd::write( m_console ? STDOUT_FILENO : m_uart, &data, 1);
}

uint SerialRPi::write( const String str)
{
    String s = str;
    return unistd::write( m_console ? STDOUT_FILENO : m_uart, s.c_str(), s.length());
}

uint SerialRPi::write( const uint8_t buf[], const uint len)
{
    return unistd::write( m_console ? STDOUT_FILENO : m_uart, &buf, len);
}

SerialRPi Serial( true);
SerialRPi Serial1( false);

//////////////////////
// END CLASS SERIAL //
//////////////////////

////////////////
// CLASS GPIO //
////////////////

gpio::gpio()
{
    for (int i=0;i<MAXPINS;i++){
        pindata[i].inUse=0;
        pindata[i].dir=0;
        pindata[i].fd=0;
    }
    status.code=0;
    status.txt="";
}

gpio::~gpio(){
    closeGpio();
}

/*Set pinmode.
 * return 0 succes -1 on error,
 * status.code:
 *       0 OK,
 *       9 Gpio pin not opened!,
 */
int gpio::pinMode(uint8_t pin, uint8_t dir){
  int sts;
  String s;
  status.code=0;
  status.txt="";
  if (pindata[pin].inUse){
    if (pindata[pin].dir != dir){
      if (Direction(pin, dir)!=0) {
        s=status.txt;
        sts=status.code;
        Unexport(pin);
        status.code=sts;
        status.txt=s;
        return -1;
      }
      pindata[pin].dir=dir;
    }
  }else{
    status.code=9;
    status.txt="Gpio pin not opened!";
    return -1;
  }
  return 0;
}

void gpio::digitalWrite(uint8_t pin, uint8_t val){
  status.code=0;
  status.txt="";
  if (pindata[pin].inUse && (pindata[pin].dir==OUTPUT)) {
    Write(pin, val);
  }
}

uint8_t gpio::digitalRead(uint8_t pin){
  int val;
  status.code=0;
  status.txt="";
  val=0;
  if (pindata[pin].inUse && (pindata[pin].dir==INPUT)){
    val=Read(pin);
  }
  return val;
}

/*Export pin.
 * return 0 succes -1 on error,
 * status.code:
 *       0 OK,
 *       8 Failed to open export for writing,
 */
int gpio::Export(uint8_t pin){
  char buffer[3];
  uint8_t bytes_written;
  int fd;
  status.code=0;
  status.txt="";
  fd = open("/sys/class/gpio/export", O_WRONLY);
  if (fd == -1){
    status.code=8;
    status.txt="Failed to open export for writing!";
    return(-1);
  }
  bytes_written = snprintf(buffer, 3, "%d", pin);
  unistd::write(fd, buffer, bytes_written);
  unistd::close(fd);
  pindata[pin].inUse=1;
  return(0);
}

/*Unexport pin.
 * return 0 succes -1 on error,
 * status.code:
 *       0 OK,
 *       7 Failed to open unexport for writing,
 */
int gpio::Unexport(uint8_t pin){
  char buffer[3];
  uint8_t bytes_written;
  int fd;
  if (pindata[pin].fd) unistd::close(pindata[pin].fd);
  fd = open("/sys/class/gpio/unexport", O_WRONLY);
  if (fd == -1){
    status.code=7;
    status.txt="Failed to open unexport for writing!";
    return(-1);
  }
  bytes_written = snprintf(buffer, 3, "%d", pin);
  unistd::write(fd, buffer, bytes_written);
  unistd::close(fd);
  pindata[pin].inUse=0;
  pindata[pin].dir=0;
  pindata[pin].fd=0;
  return(0);
}

/*write pin direction to fs gpio direction
 * input pin_number , direction
 * return 0 succes -1 on error,
 * status.code:
 *       0 OK,
 *       5 Failed to open gpio direction for writing,
 *       6 Failed to set direction
 */
int gpio::Direction(uint8_t pin, uint8_t dir){
  static const char s_directions_str[]  = "in\0out";
  char path[DIRECTION_MAX];
  int fd;
  status.code=0;
  status.txt="";
  snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
  fd = open(path, O_WRONLY);
  if (fd == -1){
    status.code=5;
    status.txt="Failed to open gpio direction for writing!";
    return(-1);
  }
  if (fd==-1) return(-1);
  if (unistd::write(fd, &s_directions_str[INPUT == dir ? 0 : 3], INPUT == dir ? 2 : 3) == -1) {
    status.code=6;
    status.txt="Failed to set direction!";
    return(-1);
  }
  unistd::close(fd);
  return(0);
}

/*Read pin value from fs
 * input pin_number to read
 * return 0/1 pin value,
 * status.code:
 *       0 OK,
 *       3 Failed to open gpio value for reading,
 *       4 Failed to read value.
 */
uint8_t gpio::Read(uint8_t pin){
  char path[VALUE_MAX];
  char value_str[3] = { 0, 0, 0};
  int fd;
  status.code=0;
  status.txt="";
  if (pindata[pin].fd==0){
    snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
    pindata[pin].path = path;
  }
  if ( pindata[pin].fd )
    unistd::close( pindata[pin].fd);
  fd = open(pindata[pin].path.c_str(), O_RDONLY);
  if (fd == -1){
      status.code=3;
      status.txt="Failed to open gpio value for reading!";
      return(0);
  }
  pindata[pin].fd=fd;
  if (unistd::read(pindata[pin].fd, value_str, 3) == -1) {
    status.code=4;
    status.txt="Failed to read value!";
    return(0);
  }
  return( atoi( value_str) );
}

/*Write pin value to fs
 * input pin_number  to write to
 * return 0 succes -1 on error,
 * status.code:
 *       0 OK,
 *       1 Failed to open gpio value for writing,
 *       2 Failed to write value
 */
int gpio::Write(uint8_t pin, uint8_t value){
  static const char s_values_str[] = "01";
  char path[VALUE_MAX];
  int fd;
  status.code=0;
  status.txt="";
  if (pindata[pin].fd==0){
    snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
    pindata[pin].path = path;
  }
  if ( pindata[pin].fd )
    unistd::close( pindata[pin].fd);
  fd = open(pindata[pin].path.c_str(), O_WRONLY);
  if (fd == -1){
    status.code=1;
    status.txt="Failed to open gpio value for writing!";
    return(-1);
  }
  pindata[pin].fd=fd;
  if (unistd::write(pindata[pin].fd, &s_values_str[LOW == value ? 0 : 1], 1) == -1){
    status.code=2;
    status.txt="Failed to write value!";
    return(-1);
  }
  return(0);
}

void gpio::closeGpio(){
  for (int i=0;i<MAXPINS;i++){
    if (pindata[i].inUse){
      Unexport(i);
    }
  }
}

int gpio::openGpio(uint8_t pin){
  status.code=0;
  status.txt="";
  if (!pindata[pin].inUse){
    if (Export(pin)!=0) {
      status.code=-1;
      status.txt="Failed to Export gpio!";
      return -1;
    }
    delay( 150);
  }
  return 0;
}
////////////////////
// END CLASS GPIO //
////////////////////

///////////////////
// ARDUINO START //
///////////////////

extern void setup();	// must be declared in the '<application>.cpp' file
extern void loop();	// must be declared in the '<application>.cpp' file
			// the <application>.cpp' file must start with: #include "Arduino.h"
/////////
// GTK //
/////////

#ifdef GTK
#include <gtk/gtk.h>

static bool g_running = true;
static GtkWidget *WINDOW;
static GtkWidget *BODY;
static GtkWidget *DRAG = NULL;
static WidgetList CURSOR;
static bool canDrag = false;
CALLBACK mouseLClick = NULL;
CALLBACK mouseRClick = NULL;
CALLBACK mouseLRelease = NULL;
CALLBACK mouseRRelease = NULL;
static uint16_t mouseXpos = 0;
static uint16_t mouseYpos = 0;
static uint16_t widgetXoffs = 0;
static uint16_t widgetYoffs = 0;
static uint16_t dragW = 0; // drag widget sizes
static uint16_t dragH = 0;
static uint16_t fieldL = 0; // drag field sizes
static uint16_t fieldR = 0;
static uint16_t fieldT = 0;
static uint16_t fieldB = 0;

struct StickyArea
{
    uint16_t	left;
    uint16_t	top;
    uint16_t	right;
    uint16_t	bottom;
    uint16_t	dx;
    uint16_t	dy;
};

LinkedList<StickyArea*> STAR;
LinkedList<WidgetList*>	TABLE;
LinkedList<uint16_t>	HALIGN;
LinkedList<uint16_t>	VALIGN;

String IMAGESTYLE =
    String( ".gtkstyle { ") +
    String( "padding: 0; ") +
    String( "}");

String EDITSTYLE =
    String( ".gtkstyle { ") +
    String( "font-size: 11px; ") +
    String( "padding-left: 5px; padding-right: 5px; padding-top: 0; padding-bottom: 0; ") +
    String( "}");

String LABELSTYLE =
    String( ".gtkstyle { ") +
    String( "font-size: 11px; ") +
    String( "padding-left: 5px; padding-right: 5px; padding-top: 0; padding-bottom: 0; ") +
    String( "}");

String BUTTONSTYLE =
    String( ".gtkstyle { ") +
    String( "font-size: 11px; ") +
    String( "padding-left: 5px; padding-right: 5px; padding-top: 0; padding-bottom: 0; ") +
    String( "}");

String CHECKSTYLE =
    String( ".gtkstyle { ") +
    String( "font-size: 11px; ") +
    String( "padding-left: 5px; padding-right: 5px; padding-top: 0; padding-bottom: 0; ") +
    String( "}");

// system
// ------

int stickyArea( uint16_t x, uint16_t y)
{
    int sticky = -1;
    uint16_t dx, dy, da;
    for ( int i = 0; i < STAR.size(); i++ ) {
        StickyArea* s = STAR.at( i);

        if ( x < s->left ) dx = s->left - x;
        else
        if ( x > s->right ) dx = x - s->right;
        else
            dx = 0;

        if ( y < s->top ) dy = s->top - y;
        else
        if ( y > s->bottom ) dy = y - s->bottom;
        else
            dy = 0;

        if ( (dx + dy) / 2 < da ) {
            da = (dx + dy) / 2;
            sticky = i;
        }
    }
    return sticky;
}

void findWidget( uint16_t x, uint16_t y)
{
    CURSOR.removeAll();
    GList *children = gtk_container_get_children( GTK_CONTAINER( BODY));
    GList *child;
    GtkAllocation rect;
    for ( child = children; child != NULL; child = child->next) {
        GtkWidget *widget = (GtkWidget*) child->data;
        gtk_widget_get_allocation( widget, &rect);
        if ( x > rect.x && x < (rect.x + rect.width) &&
             y > rect.y && y < (rect.y + rect.height) )
            CURSOR.add( widget);
    }
}

gboolean onIdle( gpointer data)
{
    loop();
    return g_running;
}

gboolean onWindowClose( GtkWidget *widget, GdkEvent *event, gpointer data)
{
    g_running = false;
    return false;
}

gboolean onMouseMove( GtkWidget *widget, GdkEventMotion *event)
{
    if ( canDrag && DRAG ) {
        uint16_t x = event->x - widgetXoffs;
        uint16_t y = event->y - widgetYoffs;
        if ( fieldR || fieldB ) {
            if ( x < fieldL ) x = fieldL;
            if ( y < fieldT ) y = fieldT;
            if ( x > fieldR - dragW ) x = fieldR - dragW;
            if ( y > fieldB - dragH ) y = fieldB - dragH;
        }

        int ix = stickyArea( x, y);
        if ( ix >= 0 ) {
            x = STAR.at( ix)->left;
            y = STAR.at( ix)->top;
        }

        gtk_fixed_move( (GtkFixed*) BODY, DRAG, x, y);
    }
    return true;
}

gboolean onMouseClick( GtkWidget *widget, GdkEventButton *event)
{
    mouseXpos = event->x;
    mouseYpos = event->y;
    if ( gtk_widget_get_window( BODY) == event->window )
        findWidget( mouseXpos, mouseYpos);
    else {
        CURSOR.removeAll();
        CURSOR.add( widget);
    }

    switch ( event->button ) {
        case 1 :    if ( mouseLClick ) mouseLClick();
                    canDrag = true;
                    break;
        case 3 :    if ( mouseRClick ) mouseRClick();
                    break;
    }
    return true;
}

gboolean onMouseRelease( GtkWidget *widget, GdkEventButton *event)
{
    mouseXpos = event->x;
    mouseYpos = event->y;
    if ( gtk_widget_get_window( BODY) == event->window )
        findWidget( mouseXpos, mouseYpos);
    else {
        CURSOR.removeAll();
        CURSOR.add( widget);
    }
    switch ( event->button ) {
        case 1 :    if ( mouseLRelease ) mouseLRelease();
                    canDrag = false;
                    break;
        case 3 :    if ( mouseRRelease ) mouseRRelease();
                    break;
    }
    return true;
}

void activate( GtkApplication *app, gpointer user_data)
{
    WINDOW = gtk_application_window_new( app);
    gtk_window_set_title( GTK_WINDOW( WINDOW), "GTK-APP");
    gtk_window_set_default_size( GTK_WINDOW( WINDOW), 250, 0);
    gtk_window_set_position( GTK_WINDOW( WINDOW), GTK_WIN_POS_CENTER);
    g_signal_connect( G_OBJECT( WINDOW), "delete-event", G_CALLBACK( onWindowClose), NULL);

    g_idle_add( onIdle, WINDOW);

    g_signal_connect( WINDOW, "motion_notify_event", G_CALLBACK( onMouseMove), NULL);
    gtk_widget_set_events( WINDOW, GDK_POINTER_MOTION_MASK);

    g_signal_connect( WINDOW, "button_press_event", G_CALLBACK( onMouseClick), NULL);
    gtk_widget_set_events( WINDOW, GDK_BUTTON_PRESS_MASK);

    g_signal_connect( WINDOW, "button_release_event", G_CALLBACK( onMouseRelease), NULL);
    gtk_widget_set_events( WINDOW, GDK_BUTTON_RELEASE_MASK);

    BODY = gtk_fixed_new();
    gtk_container_add( GTK_CONTAINER( WINDOW), BODY);
    gtk_widget_set_name( BODY, "BODY");

    g_print( "Starting 'setup'\n\n");
    setup();

    gtk_widget_show_all( WINDOW);

    g_print( "Starting 'loop'\n\n");
}

// interface
// ---------

void setTitle( String title)
{
    gtk_window_set_title( GTK_WINDOW( WINDOW), title.c_str());
}

void setSize( uint16_t width, uint16_t height)
{
    gtk_window_set_default_size( GTK_WINDOW( WINDOW), width, height);
}

void* create( uint8_t type, String name)
{
    return create( type, name, "");
}

void applyStyle( GtkWidget* widget, String cssstyle)
{
    GtkCssProvider* style;
    GtkStyleContext* context;
    style = gtk_css_provider_new();
    gtk_css_provider_load_from_data( style, cssstyle.c_str(), -1, NULL);
    context = gtk_widget_get_style_context( widget);
    gtk_style_context_add_provider( context,
        GTK_STYLE_PROVIDER( style), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_style_context_add_class( context, "gtkstyle");
    g_object_unref( style);
}

void* create( uint8_t type, String name, String param)
{
    GtkWidget *item;

    switch ( type ) {
        case tImage:	item = gtk_image_new_from_file( param.c_str());
                        applyStyle( item, IMAGESTYLE);
                        break;
        case tLabel:	item = gtk_label_new( param.c_str());
                        applyStyle( item, LABELSTYLE);
                        break;
        case tEdit:	    item = gtk_entry_new();
                        gtk_entry_set_width_chars( (GtkEntry*) item, 1);
                        gtk_entry_set_text( (GtkEntry*) item, param.c_str());
                        applyStyle( item, EDITSTYLE);
                        break;
        case tButton:	item = gtk_button_new_with_label( param.c_str()); // button text
                        applyStyle( item, BUTTONSTYLE);
			g_signal_connect( item, "button_press_event", G_CALLBACK( onMouseClick), NULL);
			gtk_widget_set_events( item, GDK_BUTTON_PRESS_MASK);
			g_signal_connect( item, "button_release_event", G_CALLBACK( onMouseRelease), NULL);
			gtk_widget_set_events( item, GDK_BUTTON_RELEASE_MASK);
                        break;
        case tCheck:	item = gtk_check_button_new_with_label( param.c_str()); // button tekst
                        applyStyle( item, CHECKSTYLE);
                        break;
//	    case tRadio:	item = gtk_radio_button_new_with_label( ); break;
    }
    gtk_widget_set_no_show_all( item, true);
    gtk_widget_set_name( item, name.c_str());
    gtk_widget_show( item);
    gtk_fixed_put( (GtkFixed*) BODY, item, 0, 0);
    return item;
}

void appendDefaultStyle( uint8_t type, String style)
{
    String st =
        String( ".gtkstyle { ") +
        String( "font-size: 11px; ") +
        String( "padding-left: 5px; padding-right: 5px; padding-top: 0; padding-bottom: 0; ") +
        style + " }";

    switch ( type ) {
        case tImage:	IMAGESTYLE = st; break;
        case tLabel:	LABELSTYLE = st; break;
        case tEdit:	    EDITSTYLE = st; break;
        case tButton:	BUTTONSTYLE = st; break;
        case tCheck:	CHECKSTYLE = st; break;
//	    case tRadio:	RADIONSTYLE = st; break;
    }
}

void setSize( void* widget, uint16_t width, uint16_t height)
{
    gtk_widget_set_size_request( (GtkWidget*) widget, width, height);
}

void place( void* widget, uint16_t x, uint16_t y)
{
    gtk_fixed_move( (GtkFixed*) BODY, (GtkWidget*) widget, x, y);
}

String name( void* widget)
{
    if ( !widget ) return "";
    return gtk_widget_get_name( (GtkWidget*) widget);
}

void destroy( void* widget)
{
    gtk_widget_destroy( (GtkWidget*) widget);
}

void show( void* widget)
{
    show( widget, true);
}

void show( void* widget, bool show)
{
    if ( show )
	gtk_widget_show( (GtkWidget*) widget);
    else
	gtk_widget_hide( (GtkWidget*) widget);
}

void hide( void* widget)
{
    gtk_widget_hide( (GtkWidget*) widget);
}

void drag( void* widget)
{
    DRAG = (GtkWidget*) widget;
    if ( widget ) {
	GtkAllocation rect;
	gtk_widget_get_allocation( DRAG, &rect);
	dragH = rect.width;
	dragW = rect.height;
    }
    else {
	dragH = 0;
	dragW = 0;
    }
}

void drop()
{
    DRAG = NULL;
}

void setDragField( uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)
{
    fieldL = left;
    fieldT = top;
    fieldR = right;
    fieldB = bottom;
}

void setText( void* widget, uint8_t type, String text)
{
    switch ( type ) {
	case tEdit:	gtk_entry_set_text( (GtkEntry*) widget, text.c_str()); break;
	case tLabel:	gtk_label_set_text( (GtkLabel*) widget, text.c_str()); break;
    }
}

String text( void* widget, uint8_t type)
{
    switch ( type ) {
	case tEdit:	return gtk_entry_get_text( (GtkEntry*) widget);
	case tLabel:	return gtk_label_get_text( (GtkLabel*) widget);
    }
    return "";
}

void setCheck( void* widget, bool check)
{
    gtk_toggle_button_set_active( (GtkToggleButton*) widget, check);
}

bool isChecked( void* widget)
{
    return gtk_toggle_button_get_active( (GtkToggleButton*) widget);
}

uint16_t mouseX()
{
    return mouseXpos;
}

uint16_t mouseY()
{
    return mouseYpos;
}

void *mouseWidget( String wname)
{
    int ix = -1;
    if ( wname == "" && CURSOR.size() )
        ix = 0;
    else
        for ( ix = CURSOR.size() - 1; ix >= 0; ix-- )
            if ( wname == name( CURSOR.at( ix)) )
                break;
    if ( ix >= 0 ) {
        GtkAllocation rect;
	GtkWidget* widget = (GtkWidget*) CURSOR.at( ix);
        gtk_widget_get_allocation( widget, &rect);
        widgetXoffs = mouseXpos - rect.x;
        widgetYoffs = mouseYpos - rect.y;
        return widget;
    }
    return NULL;
}

void mouse( uint16_t &x, uint16_t &y)
{
    x = mouseXpos;
    y = mouseYpos;
}

void callOnMouseLClick( CALLBACK routine)
{
    mouseLClick = routine;
}

void callOnMouseRClick( CALLBACK routine)
{
    mouseRClick = routine;
}

void callOnMouseLRelease( CALLBACK routine)
{
    mouseLRelease = routine;
}

void callOnMouseRRelease( CALLBACK routine)
{
    mouseRRelease = routine;
}

uint8_t addStickyArea( uint16_t left, uint16_t top, uint16_t width, uint16_t height)
{
    StickyArea* s = new StickyArea;
    s->left = left;
    s->top = top;
    s->right = left + width;
    s->bottom = top + height;
    return STAR.add( s);
}

void removeStickyArea( uint16_t ix)
{
    if  ( ix < STAR.size() ) {
	delete STAR.at( ix);
	STAR.removeAt( ix);
    }
}

void clearStickyArea()
{
    STAR.removeAll();
}

void addRow( WidgetList* widgets)
{
    TABLE.add( widgets);
}

void setRowValues( uint8_t row, StringList &values, uint8_t type)
{
    if ( row < TABLE.size() ) {
	WidgetList* r = TABLE.at( row);
	for ( int i = 0; i < r->size() && i < values.size(); i++ )
	    setText( r->at( i), type, values.at( i));
    }
}

void getRowValues( uint8_t row, StringList& values, uint8_t type)
{
    if ( row < TABLE.size() ) {
	WidgetList* r = TABLE.at( row);
	for ( int i = 0; i < r->size(); i++ )
	    values.add( text( r->at( i), type));
    }
}
#endif

//////////
// MAIN //
//////////


#ifdef GTK
int main( int argc, char **argv)
{
    g_start = chrono::high_resolution_clock::now();

    GtkApplication *app;
    int status;

    app = gtk_application_new( "nl.devicelib", G_APPLICATION_FLAGS_NONE);
    g_signal_connect( app, "activate", G_CALLBACK( activate), NULL);
    status = g_application_run( G_APPLICATION( app), argc, argv);
    g_object_unref( app);

    return status;
}
#else
int main( int argc, char **argv)
{
    pwmgo[0].pwmworker = pwm0loop;
    pwmgo[1].pwmworker = pwm1loop;
    pwmgo[2].pwmworker = pwm2loop;
    pwmgo[3].pwmworker = pwm3loop;
    pwmgo[4].pwmworker = pwm4loop;
    for ( int i = 0; i < pwmgoMax; i++ ) {
	pwmgo[i].pin = -1;
	pwmgo[i].high = -1;
	pwmgo[i].low = -1;
	pwmgo[i].go = false;
    }

    g_start = chrono::high_resolution_clock::now();

    printf( "Starting 'setup'\n\n");
    setup();
    printf( "Starting 'loop'\n\n");
    while ( 1 ) loop();
}
#endif
