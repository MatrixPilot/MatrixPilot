#include "stdafx.h"
#include "SerialIO.h"

HANDLE hComms;
DWORD dwRetFlag;

extern string	CommPortString;
extern int gotPacket;


//---------------------------------------------------------------------------
void OpenComms(void)
{
    DCB Dcb;
    COMMTIMEOUTS CommTimeouts;
    char ErrorString[80];
	
    if (hComms == 0)
    {
		string port = "\\\\.\\";
		port.append(CommPortString);
		
        hComms = CreateFile(port.data(),
                            GENERIC_READ | GENERIC_WRITE,
                            0,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
                            NULL);
		
        if (hComms == INVALID_HANDLE_VALUE)
        {
            sprintf(ErrorString, "CreateFile Error = %d", GetLastError());
            ShowMessage(ErrorString);
			LoggingFile.mLogFile << "Could not open Com Port";
			LoggingFile.mLogFile << endl;
        }
        else
        {
			
            dwRetFlag = GetCommState(hComms, &Dcb);
			
            if (!dwRetFlag)
            {
                sprintf(ErrorString, "GetCommState Error = %d", GetLastError());
                ShowMessage(ErrorString);
            }
			
            Dcb.DCBlength = sizeof(Dcb);
			
            Dcb.BaudRate = CommPortSpeed;
			
            Dcb.ByteSize = 8;
            Dcb.Parity = NOPARITY;
            Dcb.StopBits = ONESTOPBIT;
            Dcb.fTXContinueOnXoff = true;
			
            Dcb.fOutxCtsFlow = false;//true;                  // disable CTS output flow control
            Dcb.fOutxDsrFlow = false;                  // disable DSR output flow control
            Dcb.fDtrControl = DTR_CONTROL_HANDSHAKE  /*DTR_CONTROL_DISABLE DTR_CONTROL_ENABLE*/;
            Dcb.fDsrSensitivity = false;               // enable DSR sensitivity
			
            Dcb.fOutX = false;                        // disable XON/XOFF out flow control
            Dcb.fInX = false;                         // disable XON/XOFF in flow control
            Dcb.fErrorChar = false;                   // disable error replacement
            Dcb.fNull = false;                        // disable null stripping
            Dcb.fRtsControl = RTS_CONTROL_HANDSHAKE	  /* RTS_CONTROL_ENABLE  RTS_CONTROL_DISABLE*/;   //  enable RTS line
            Dcb.fAbortOnError = true;                 // don't abort reads/writes on error
			
            dwRetFlag = SetCommState(hComms, &Dcb);
            if (!dwRetFlag)
            {
                sprintf(ErrorString, "SetCommState Error = %d", GetLastError());
                ShowMessage(ErrorString);
            }
			
            dwRetFlag = GetCommTimeouts(hComms, &CommTimeouts);
            if (!dwRetFlag)
            {
                sprintf(ErrorString, "GetCommTimeouts Error = %d", GetLastError());
                ShowMessage(ErrorString);
            }
			
            CommTimeouts.ReadIntervalTimeout         = -1;    //Don't use interval timeouts
            CommTimeouts.ReadTotalTimeoutMultiplier  = 0;			//Don't use multipliers
            CommTimeouts.ReadTotalTimeoutConstant    = 0;			//150ms total read timeout
            CommTimeouts.WriteTotalTimeoutMultiplier = 0;			//Don't use multipliers
            CommTimeouts.WriteTotalTimeoutConstant   = 0;			//2200ms total write timeout
			
            dwRetFlag = SetCommTimeouts(hComms, &CommTimeouts);
            if (!dwRetFlag)
            {
                sprintf(ErrorString, "SetCommTimeouts Error = %d", GetLastError());
                ShowMessage(ErrorString);
            }
        }
    }
    else
    {
        ShowMessage("Comm port already open");
    }
}
//---------------------------------------------------------------------------

void CloseComms(void)
{
    if (hComms != 0)
    {
        CloseHandle(hComms);
        hComms = 0;
    }
    else
    {
        ShowMessage("Comm port already closed");
    }
}
//---------------------------------------------------------------------------

void SendToComPort(unsigned long ResponseLength, unsigned char *Buffer)
{
    unsigned long dwBytesWritten;
	
	if (hComms != 0)
        dwRetFlag = WriteFile(hComms, Buffer, ResponseLength, &dwBytesWritten, NULL);
    else
    {
        ShowMessage("Comm port not open");
    }
}
//---------------------------------------------------------------------------
void ReceiveFromComPort(void)
{
	BYTE Byte = 0x00;
	unsigned long dwBytesTransferred = 0;
	
	if(hComms != INVALID_HANDLE_VALUE) 
	{
		// Loop for waiting for the data.
		do 
		{
			// Read the data from the serial port.
			if (ReadFile(hComms, &Byte, 1, &dwBytesTransferred, 0))
			{
				if(dwBytesTransferred != 0)
				{
					HandleMsgByte(Byte);
				};
			}
			else
			{
				COMSTAT comStat;
				unsigned long   dwErrors;
				bool    fOOP, fOVERRUN, fPTO, fRXOVER, fRXPARITY, fTXFULL;
				bool    fBREAK, fDNS, fFRAME, fIOE, fMODE;
				
				// Get and clear current errors on the port.
				if (!ClearCommError(hComms, &dwErrors, &comStat))
					// Report error in ClearCommError.
					return;
				
				// Get error flags.
				fDNS = dwErrors & CE_DNS;
				fIOE = dwErrors & CE_IOE;
				fOOP = dwErrors & CE_OOP;
				fPTO = dwErrors & CE_PTO;
				fMODE = dwErrors & CE_MODE;
				fBREAK = dwErrors & CE_BREAK;
				fFRAME = dwErrors & CE_FRAME;
				fRXOVER = dwErrors & CE_RXOVER;
				fTXFULL = dwErrors & CE_TXFULL;
				fOVERRUN = dwErrors & CE_OVERRUN;
				fRXPARITY = dwErrors & CE_RXPARITY;
				
				//The only reason i left these if statements in was so i had
				// somewhere to set breakpoints when debugging the serial port stuff.
				
				
				/*			// COMSTAT structure contains information regarding
				 // communications status.
				 if (comStat.fCtsHold);
				 // Tx waiting for CTS signal
				 if (comStat.fDsrHold);
				 // Tx waiting for DSR signal
				 if (comStat.fRlsdHold);
				 // Tx waiting for RLSD signal
				 if (comStat.fXoffHold);
				 // Tx waiting, XOFF char rec'd
				 if (comStat.fXoffSent);
				 // Tx waiting, XOFF char sent
				 if (comStat.fEof);
				 // EOF character received
				 if (comStat.fTxim);
				 // Character waiting for Tx; char queued with TransmitCommChar
				 if (comStat.cbInQue);
				 // comStat.cbInQue bytes have been received, but not read
				 if (comStat.cbOutQue);
				 // comStat.cbOutQue bytes are awaiting transfer	
				 */			
			}
		} while (dwBytesTransferred == 1);
	}
	return;
}

//---------------------------------------------------------------------------
void ShowMessage(const char *pErrorString)
{
    MessageBox(NULL, pErrorString, "Error", MB_OK);
}

