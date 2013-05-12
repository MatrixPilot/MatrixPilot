//#include <p33Exxxx.h>
#include "p30sim.h"
//#include <stdio.h>
//#include "simio.h"

extern int __C30_UART;

extern volatile UxMODEBITS U3MODEbits __attribute__((__sfr__,weak));
extern volatile UxSTABITS U3STAbits __attribute__((__sfr__,weak));
extern volatile unsigned int U3TXREG __attribute__((__sfr__,weak));
extern volatile unsigned int U3BRG __attribute__((__sfr__,weak));

extern volatile UxMODEBITS U4MODEbits __attribute__((__sfr__,weak));
extern volatile UxSTABITS U4STAbits __attribute__((__sfr__,weak));
extern volatile unsigned int U4TXREG __attribute__((__sfr__,weak));
extern volatile unsigned int U4BRG __attribute__((__sfr__,weak));

//static void __inline__ dowrite(PSIMIO psimio) {
//  __asm__(".pword 0xDAC000" :: "a" (psimio) : "memory");
//}

int __attribute__((__weak__, __section__(".libc")))
write(int handle, void *buffer, unsigned int len) 
{
  int i;
  volatile UxMODEBITS *umode = &U1MODEbits;
  volatile UxSTABITS *ustatus = &U1STAbits;
  volatile unsigned int *txreg = &U1TXREG;
  volatile unsigned int *brg = &U1BRG;

  switch (handle)
  {
    case 0:
    case 1:
    case 2:
//      if ((__C30_UART != 1) && (&U3BRG)) {
//        umode = &U3MODEbits;
//        ustatus = &U3STAbits;
//        txreg = &U3TXREG;
//        brg = &U3BRG;
//      }
      if (__C30_UART == 2) {
        umode = &U2MODEbits;
        ustatus = &U2STAbits;
        txreg = &U2TXREG;
        brg = &U2BRG;
      }
      if ((__C30_UART == 3) && (&U3BRG)) {
        umode = &U3MODEbits;
        ustatus = &U3STAbits;
        txreg = &U3TXREG;
        brg = &U3BRG;
      }
      if ((__C30_UART == 4) && (&U4BRG)) {
        umode = &U4MODEbits;
        ustatus = &U4STAbits;
        txreg = &U4TXREG;
        brg = &U4BRG;
      }
      if ((umode->UARTEN) == 0)
      {
        *brg = 0;
        umode->UARTEN = 1;
      }
      if ((ustatus->UTXEN) == 0)
      {
        ustatus->UTXEN = 1;
      }
      for (i = len; i; --i)
      {
        while ((ustatus->TRMT) ==0);
        *txreg = *(char*)buffer++;
      }
      break;

    default: {
/*
      SIMIO simio;
      register PSIMIO psimio asm("w0") = &simio;

      simio.method = SIM_WRITE;
      simio.u.write.handle = handle;
      simio.u.write.buffer = buffer;
      simio.u.write.len = len;
      dowrite(psimio);
      len = simio.u.write.len;
 */
      break;
    }
  }
  return(len);
}

