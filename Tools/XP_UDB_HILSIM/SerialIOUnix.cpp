#include "SerialIO.h"
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int				fd;
struct termios  config;

extern LogFile LoggingFile;
extern string	CommPortString;
extern long CommPortSpeed;


//---------------------------------------------------------------------------
void OpenComms(void)
{
	speed_t BAUD;
	const char *device = CommPortString.c_str();
	fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1) {
		LoggingFile.mLogFile << "failed to open port:" << endl;
		ShowMessage(device);
		CloseComms();
		return;
	}
	
	if (!isatty(fd)) {
		LoggingFile.mLogFile << "not a tty:" << endl;
		ShowMessage(device);
		CloseComms();
		return;
	}
	
	if (tcgetattr(fd, &config) < 0) {
		LoggingFile.mLogFile << "failed to get port info" << endl;
		CloseComms();
		return;
	}
	
	//
	// Input flags - Turn off input processing
	// convert break to null byte, no CR to NL translation,
	// no NL to CR translation, don't mark parity errors or breaks
	// no input parity check, don't strip high bit off,
	// no XON/XOFF software flow control
	//
	config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | 
						INLCR | PARMRK | INPCK | ISTRIP | IXON);
	//
	// Output flags - Turn off output processing
	// no CR to NL translation, no NL to CR-NL translation,
	// no NL to CR translation, no column 0 CR suppression,
	// no Ctrl-D suppression, no fill characters, no case mapping,
	// no local output processing
	//
	// config.c_oflag &= ~(OCRNL | ONLCR | ONLRET |
	//                     ONOCR | ONOEOT| OFILL | OLCUC | OPOST);
	config.c_oflag = 0;
	
	//
	// No line processing:
	// echo off, echo newline off, canonical mode off, 
	// extended input processing off, signal chars off
	//
	config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
	
	//
	// Turn off character processing
	// clear current char size mask, no parity checking,
	// no output processing, force 8 bit input
	//
	config.c_cflag &= ~(CSIZE | PARENB | CSTOPB);
	config.c_cflag |= CS8;
	
	//
	// One input byte is enough to return from read()
	// Inter-character timer off
	//
	config.c_cc[VMIN]  = 1;
	config.c_cc[VTIME] = 0;
	
	//
	// Communication speed (simple version, using the predefined
	// constants)
	//
#if ( LIN == 1 )
	switch (CommPortSpeed)
	      {
		case 1152000:
                  BAUD = 1152000;
		  break;
		case 1000000:
		  BAUD = 1000000;
		  break;
		case 921600:
                  BAUD = 921600;
		  break;
		case 576000:
		  BAUD = 576000;
		  break;
		case 500000:
		  BAUD = 500000;
		  break;
		case 460800:
		  BAUD = 460800;
                  break;
		case 230400:
		  BAUD = B230400;
		  break;
		case 115200:
		  BAUD = B115200;
                  break;
		case 57600:
		  BAUD = B57600;
                  break;
		 case 38400:
		    BAUD = B38400;
		    break;
		 case 19200:
		    BAUD  = B19200;
		    break;
		 case 9600:
		    BAUD  = B9600;
		    break;
		 case 4800:
		    BAUD  = B4800;
		    break;
		 case 2400:
		    BAUD  = B2400;
		    break;
		 case 1800:
		    BAUD  = B1800;
		    break;
		 case 1200:
		    BAUD  = B1200;
		    break;
		 case 600:
		    BAUD  = B600;
		    break;
		 case 300:
		    BAUD  = B300;
		    break;
		 case 200:
		    BAUD  = B200;
		    break;
		 case 150:
		    BAUD  = B150;
		    break;
		 case 134:
		    BAUD  = B134;
		    break;
		 case 110:
		    BAUD  = B110;
		    break;
		 case 75:
		    BAUD  = B75;
		    break;
		 case 50:
		    BAUD  = B50;
		    break;
		 default:
		    BAUD = B19200;
		    break;

	      }  //end of switch CommPortSpeed
	if (cfsetispeed(&config, BAUD) < 0 || cfsetospeed(&config, BAUD) < 0) {
#else
	if (cfsetispeed(&config, CommPortSpeed) < 0 || cfsetospeed(&config, CommPortSpeed) < 0) {
#endif
		LoggingFile.mLogFile << "failed to set port speed" << endl;
		CloseComms();
		return;
	}
	
	//
	// Finally, apply the configuration
	//
	if (tcsetattr(fd, TCSAFLUSH, &config) < 0) {
		LoggingFile.mLogFile << "failed to configure port" << endl;
		CloseComms();
		return;
	}
	
	LoggingFile.mLogFile << "Opened port " << device << endl;
}
//---------------------------------------------------------------------------


void CloseComms(void)
{
	if (fd != -1) {
		close(fd);
		fd = -1;
		
		LoggingFile.mLogFile << "Closed port" << endl;
	}
}
//---------------------------------------------------------------------------


void SendToComPort(unsigned long ResponseLength, unsigned char *Buffer)
{
	if (fd != -1) {
		int written = write(fd, Buffer, ResponseLength);
		
		if (written != ResponseLength) {
			LoggingFile.mLogFile << "serial write failed (" << ResponseLength << ", " << written << ")" << endl;
		}
	}
}
//---------------------------------------------------------------------------


void ReceiveFromComPort(void)
{
	unsigned long dwBytesTransferred = 0;
	char Byte = 0x00;
	
	if (fd != -1) 
	{
		// Loop for waiting for the data.
		while (read(fd, &Byte, 1) == 1)
		{
			HandleMsgByte(Byte);
		}
	}
}
//---------------------------------------------------------------------------


void ShowMessage(const char *pErrorString)
{
	LoggingFile.mLogFile << "MESSAGE: " << pErrorString << endl;
	cerr << "MESSAGE: " << pErrorString << endl;
}
