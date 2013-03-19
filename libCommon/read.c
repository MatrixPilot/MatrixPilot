//#include <p30sim.h>
#include "p30sim.h"
#include <stdio.h>
//#include "simio.h"

extern int __C30_UART;

extern volatile UxMODEBITS U3MODEbits __attribute__((__sfr__,weak));
extern volatile UxSTABITS U3STAbits __attribute__((__sfr__,weak));
extern volatile unsigned int U3RXREG __attribute__((__sfr__,weak));
extern volatile unsigned int U3BRG __attribute__((__sfr__,weak));

int __attribute__((__weak__, __section__(".libc")))
read(int handle, void *buffer, unsigned int len)
{
  int i;
  volatile UxMODEBITS *umode = &U1MODEbits;
  volatile UxSTABITS *ustatus = &U1STAbits;
  volatile unsigned int *rxreg = &U1RXREG;
  volatile unsigned int *brg = &U1BRG;

  switch (handle)
  {
    case 0:
#ifdef __C30_LEGACY_LIBC__
      if (_Files[0]->_Lockno == 0)
#endif
      {
        if ((__C30_UART != 1) && (&U3BRG)) {
          umode = &U3MODEbits;
          ustatus = &U3STAbits;
          rxreg = &U3RXREG;
          brg = &U3BRG;
        }
        if ((umode->UARTEN) == 0)
        {
          *brg = 0;
          umode->UARTEN = 1;
        }
        for (i = len; i; --i)
        {
          int nTimeout;

          /*
          ** Timeout is 16 cycles per 10-bit char
          */
          nTimeout = 16*10;
          while (((ustatus->URXDA) == 0) && nTimeout) --nTimeout;
          if ((ustatus->URXDA) == 0) break;
          *(char*)buffer++ = *rxreg;
        }
        len -= i;
        break;
      }

    default: {
/*
      SIMIO simio;
      register PSIMIO psimio asm("w0");

      psimio  = &simio;
      simio.method = SIM_READ;
      simio.u.read.handle = handle;
      simio.u.read.buffer = buffer;
      simio.u.read.len = len;
      doread(psimio);

      len = simio.u.read.len;
 */
      break;
    }
  }
  return(len);
}
