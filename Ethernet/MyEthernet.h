
#ifndef _MYETHERNET_H_
#define _MYETHERNET_H_


#define BAUD_RATE       (19200)		// bps

extern unsigned char AN0String[8];

   
   // Private helper functions.
	// These may or may not be present in all applications.
void ServiceMyEthernet(void);
void InitMyEthernet(void);


// An actual function defined in MainDemo.c for displaying the current IP 
// address on the UART and/or LCD.

#endif // _MYETHERNET_H_

