// ArduinoCore.cpp

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

#include <chrono>
#include <termio.h>
#include "ArduinoCore.h"
#include "Wire.h"

namespace unistd {
	//All functions of unistd.h must be called like this: unistd::the_function()
    #include <unistd.h>
};

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


hirestime g_start;

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

void pinMode( int pin, int mode)
{
    int fsel;
    switch ( mode ) {
	case OUTPUT :		fsel = BCM2835_GPIO_FSEL_OUTP; break;
	case INPUT :		fsel = BCM2835_GPIO_FSEL_INPT; break;
	case INPUT_PULLUP :	fsel = BCM2835_GPIO_FSEL_INPT;
				bcm2835_gpio_set_pud( pin, BCM2835_GPIO_PUD_UP);
				break;
	case INPUT_PULLDOWN :	fsel = BCM2835_GPIO_FSEL_INPT;
				bcm2835_gpio_set_pud( pin, BCM2835_GPIO_PUD_DOWN);
				break;
    }
    bcm2835_gpio_fsel( pin, fsel);
}

void analogWrite( int pin, int level)
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

extern void setup();	// must be declared in the '<application>.cpp' file
extern void loop();	// must be declared in the '<application>.cpp' file
			// the <application>.cpp' file must start with: #include "Arduino.h"

int main( int argc, char **argv)
{
    if ( !bcm2835_init() ) {
	printf( "Can not initialise bcm2835 library.\nProgram will be aborted.\n");
	bcm2835_close();
	return 1;
    }

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

    bcm2835_close();
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

byte SerialRPi::readB( const char stop, byte data[], const byte len, bool dostop)
{
    memset( data, 0, len);
    char chr;
    byte cnt = 0;
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

byte SerialRPi::readBytes( byte data[], const byte len)
{
    return readB( 0, data, len, false);
}

byte SerialRPi::readBytesUntil( const char stop, byte data[], const byte len)
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

uint SerialRPi::write( const byte data)
{
    return unistd::write( m_console ? STDOUT_FILENO : m_uart, &data, 1);
}

uint SerialRPi::write( const String str)
{
    String s = str;
    return unistd::write( m_console ? STDOUT_FILENO : m_uart, s.c_str(), s.length());
}

uint SerialRPi::write( const byte buf[], const uint len)
{
    return unistd::write( m_console ? STDOUT_FILENO : m_uart, &buf, len);
}

SerialRPi Serial( true);
SerialRPi Serial1( false);

//////////////////////
// END CLASS SERIAL //
//////////////////////


/*
*  Copyright (C) 2012 Libelium Comunicaciones Distribuidas S.L.
*  http://www.libelium.com
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*  Version 2.4 (For Raspberry Pi 2)
*  Author: Sergio Martinez, Ruben Martin
*/

#include <poll.h>
#include "ArduinoCore.h"

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

int analogRead (int pin){

	int value;
	char selected_channel[1];
	char read_values[2];

	if (pin == 0) {
		selected_channel[0] = 0xDC;
	} else if (pin == 1){
		selected_channel[0] = 0x9C;
	} else if (pin == 2){ 
		selected_channel[0] = 0xCC ;
	} else if (pin == 3){ 
		selected_channel[0] = 0x8C;
	} else if (pin == 4){ 
		selected_channel[0] = 0xAC;
	} else if (pin == 5){ 
		selected_channel[0] = 0xEC;
	} else if (pin == 6){ 
		selected_channel[0] = 0xBC;
	} else if (pin == 7){ 
		selected_channel[0] = 0xFC;
	}
	
	Wire.begin();
	Wire.beginTransmission(8); 
	Wire.read_rs(selected_channel, read_values, 2);
	Wire.read_rs(selected_channel, read_values, 2);

	value = int(read_values[0])*16 + int(read_values[1]>>4);
	value = value * 1023 / 4095;  //mapping the value between 0 and 1023
	return value;
}

void attachInterrupt(int p,void (*f)(), uint m){
	int GPIOPin = raspberryPinNumber(p);
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

void detachInterrupt(int p){
	int GPIOPin = raspberryPinNumber(p);
	
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

// Helper routines

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
