#if defined(__dsPIC33E__)
#include <p33Exxxx.h>
#elif defined(__dsPIC33F__)
#include <p33Fxxxx.h>
#endif

extern int __C30_UART;

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
			if (__C30_UART == 2) {
				umode = (UxMODEBITS*)&U2MODEbits;
				ustatus = (UxSTABITS*)&U2STAbits;
				txreg = &U2TXREG;
				brg = &U2BRG;
			}
#if defined (__dsPIC33E__)
			if (__C30_UART == 3) {
				umode = (UxMODEBITS*)&U3MODEbits;
				ustatus = (UxSTABITS*)&U3STAbits;
				txreg = &U3TXREG;
				brg = &U3BRG;
			}
			if (__C30_UART == 4) {
				umode = (UxMODEBITS*)&U4MODEbits;
				ustatus = (UxSTABITS*)&U4STAbits;
				txreg = &U4TXREG;
				brg = &U4BRG;
			}
#endif // __dsPIC33E__
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
	return len;
}
