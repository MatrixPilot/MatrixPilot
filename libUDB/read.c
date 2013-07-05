#if defined(__dsPIC33E__)
#include <p33Exxxx.h>
#elif defined(__dsPIC33F__)
#include <p33Fxxxx.h>
#endif

extern int __C30_UART;

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
		case 1:
		case 2:
#ifdef __C30_LEGACY_LIBC__
			if (_Files[0]->_Lockno == 0)
#endif
			{
			if (__C30_UART == 2) {
				umode = (UxMODEBITS*)&U2MODEbits;
				ustatus = (UxSTABITS*)&U2STAbits;
				rxreg = &U2RXREG;
				brg = &U2BRG;
			}
#if defined (__dsPIC33E__)
			if (__C30_UART == 3) {
				umode = (UxMODEBITS*)&U3MODEbits;
				ustatus = (UxSTABITS*)&U3STAbits;
				rxreg = &U3RXREG;
				brg = &U3BRG;
			}
			if (__C30_UART == 4) {
				umode = (UxMODEBITS*)&U4MODEbits;
				ustatus = (UxSTABITS*)&U4STAbits;
				rxreg = &U4RXREG;
				brg = &U4BRG;
			}
#endif // __dsPIC33E__
				if ((umode->UARTEN) == 0)
				{
					*brg = 0;
					umode->UARTEN = 1;
				}
				for (i = len; i; --i)
				{
					int nTimeout;

					// Timeout is 16 cycles per 10-bit char
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

			psimio = &simio;
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
	return len;
}
