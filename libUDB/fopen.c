#if defined(__dsPIC33E__)
#include <p33Exxxx.h>
#elif defined(__dsPIC33F__)
#include <p33Fxxxx.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <string.h>


#ifdef __C30_LEGACY_LIBC__

void print_iob(char* name, FILE* iob)
{
	printf("%s - iob: %p\r\n", name, iob);
	printf("\t_Mode %u\r\n", iob->_Mode);
	printf("\t_Lockno %u\r\n", iob->_Lockno);
	printf("\t_Handle %u\r\n", iob->_Handle);
	printf("\t_Buf   %p\r\n", iob->_Buf);
	printf("\t_Bend  %p\r\n", iob->_Bend);
	printf("\t_Next  %p\r\n", iob->_Next);
	printf("\t_Rend  %p\r\n", iob->_Rend);
	printf("\t_Wend  %p\r\n", iob->_Wend);
	printf("\t_Rback %p\r\n", iob->_Rback);

//	printf("\t_WRback %p\r\n", iob->_WRback);
//	printf("\t_WBack[0] %u\r\n", iob->_WBack[0]);
//	printf("\t_WBack[1] %u\r\n", iob->_WBack[1]);

//	printf("\t_Rsave %p\r\n", iob->_Rsave);
//	printf("\t_WRend %p\r\n", iob->_WRend);
//	printf("\t_WWend %p\r\n", iob->_WWend);

	printf("\t_Tmpnam %p\r\n", iob->_Tmpnam);
//	printf("\t_Back[0] %u\r\n", iob->_Back[0]);
//	printf("\t_Back[1] %u\r\n", iob->_Back[1]);
	printf("\t_Cbuf %u\r\n", iob->_Cbuf);
}

#endif // __C30_LEGACY_LIBC__

#if 1

#ifdef __C30_LEGACY_LIBC__

/*
typedef struct _Filet
	{
	unsigned short _Mode;
	unsigned char _Lockno;
	_FD_TYPE _Handle;

	unsigned char *_Buf, *_Bend, *_Next;
	unsigned char *_Rend, *_Wend, *_Rback;

	_Wchart *_WRback, _WBack[2];
	unsigned char *_Rsave, *_WRend, *_WWend;

	struct _Mbstatet _Wstate;
	char *_Tmpnam;
	unsigned char _Back[_MBMAX * 2], _Cbuf;
	} FILE;
 */

//#define offsetof(st, m) ((size_t)(&((st *)0)->m))

static void init_iob(FILE* iob, _FD_TYPE handle, unsigned short mode)
{
	memset(iob, 0, sizeof(FILE));
	iob->_Mode   = mode;
	iob->_Lockno = 1;
	iob->_Handle = handle;
	iob->_Buf    = iob->_Back;
	iob->_Bend   = iob->_Back + 1;
	iob->_Next   = iob->_Back;
	iob->_Rend   = iob->_Back;
	iob->_Wend   = iob->_Back;
	iob->_Rback  = iob->_Back;
}

static FILE _iob[4];

FILE* fopen(const char* filename, const char* mode)
{
	_FD_TYPE handle = 0;
    FILE* fp = NULL;

	if (filename[0] == 'c' && filename[1] == 'o' && filename[2] == 'm')
	{
		handle = filename[3] - '0';
		if (handle > 0 && handle < 4)
		{
			fp = &_iob[handle - 1];
//			init_iob(fp, handle, mode[0] << 8 + mode[1]);
			init_iob(fp, handle, stdout->_Mode);
		}
	}
	return fp;
}
/*
//	print_iob("stdout", stdout);
	memset(&uart_iob, 0, sizeof(uart_iob));

	uart_iob._Mode = stdout->_Mode;
	uart_iob._Lockno = stdout->_Lockno;
	uart_iob._Handle = 2;
	uart_iob._Buf = uart_iob._Back;
	uart_iob._Bend = uart_iob._Back + 1;
	uart_iob._Next = uart_iob._Back;
	uart_iob._Rend = uart_iob._Back;
	uart_iob._Wend = uart_iob._Back;
	uart_iob._Rback = uart_iob._Back;

//	print_iob("uart_iob", &uart_iob);

	return &uart_iob;
 */

#else
/*
extern	struct	_iobuf {
	char *		_ptr;
	int		_cnt;
	char *		_base;
	unsigned short	_flag;
	short		_file;
	size_t		_size;
} _iob[_NFILE];
 */
//#define	FILE		struct _iobuf
//	uart_iob._ptr = NULL;
//	uart_iob._cnt = 0;
//	uart_iob._base = NULL;
//	uart_iob._flag = 0;
//	uart_iob._file = 5;
//	uart_iob._size = 0;
#endif // __C30_LEGACY_LIBC__

#endif
