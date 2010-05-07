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
extern int gotPacket;


//---------------------------------------------------------------------------
void OpenComms(void)
{
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
	if (cfsetispeed(&config, B19200) < 0 || cfsetospeed(&config, B19200) < 0) {
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
		while (gotPacket < 2) {
			if (read(fd, &Byte, 1) == 1)
			{
				HandleMsgByte(Byte);
				LoggingFile.mLogFile << ".";
			}
		}
		gotPacket = 0;
		LoggingFile.mLogFile << endl;
	}
}
//---------------------------------------------------------------------------


void ShowMessage(const char *pErrorString)
{
	LoggingFile.mLogFile << "MESSAGE: " << pErrorString << endl;
	cerr << "MESSAGE: " << pErrorString << endl;
}
