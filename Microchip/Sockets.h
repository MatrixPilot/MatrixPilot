#ifndef _SOCKETS_H_
#define _SOCKETS_H_


typedef struct
{
	BYTE vSocketPurpose;
	BYTE vMemoryMedium;
	WORD wTXBufferSize;
	WORD wRXBufferSize;
} TCPSocketInitializer_t;


#endif // _SOCKETS_H_
