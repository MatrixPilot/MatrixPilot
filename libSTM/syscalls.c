/* Support files for GNU libc.  Files in the system namespace go here.
   Files in the C namespace (ie those that do not start with an
   underscore) go in .c.  */

#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#include <reent.h>
#include <unistd.h>
#include <sys/wait.h>

#include "stm32f4xx_hal.h"
#include "usart.h"

#undef errno
extern int errno;

#define FreeRTOS
#define MAX_STACK_SIZE 0x200

extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));

#ifndef FreeRTOS
  register char * stack_ptr asm("sp");
#endif




caddr_t _sbrk(int incr)
{
	extern char end asm("end");
	static char *heap_end;
	char *prev_heap_end,*min_stack_ptr;

	if (heap_end == 0)
		heap_end = &end;

	prev_heap_end = heap_end;

#ifdef FreeRTOS
	/* Use the NVIC offset register to locate the main stack pointer. */
	min_stack_ptr = (char*)(*(unsigned int *)*(unsigned int *)0xE000ED08);
	/* Locate the STACK bottom address */
	min_stack_ptr -= MAX_STACK_SIZE;

	if (heap_end + incr > min_stack_ptr)
#else
	if (heap_end + incr > stack_ptr)
#endif
	{
//		write(1, "Heap and stack collision\n", 25);
//		abort();
		errno = ENOMEM;
		return (caddr_t) -1;
	}

	heap_end += incr;

	return (caddr_t) prev_heap_end;
}

/*
 * _gettimeofday primitive (Stub function)
 * */
int _gettimeofday (struct timeval * tp, struct timezone * tzp)
{
  /* Return fixed data for the timezone.  */
  if (tzp)
    {
      tzp->tz_minuteswest = 0;
      tzp->tz_dsttime = 0;
    }

  return 0;
}

//void initialise_monitor_handles()
//{
//}

int _getpid(void)
{
	return 1;
}

int _kill(int pid, int sig)
{
	errno = EINVAL;
	return -1;
}

void _exit (int status)
{
	_kill(status, -1);
	while (1) {}
}

#if 0
UART_HandleTypeDef UartHandle;

void init_uart_stdio(unsigned long baud)
{
	/*##-1- Configure the UART peripheral ######################################*/
	/* Put the USART peripheral in the Asynchronous mode (UART Mode) */
	/* UARTx configured as follow:
		- Word Length = 8 Bits
		- Stop Bit = One Stop bit
		- Parity = None
		- BaudRate = 9600 baud
		- Hardware flow control disabled (RTS and CTS signals) */
	UartHandle.Instance        = UART2;
	UartHandle.Init.BaudRate   = baud;
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits   = UART_STOPBITS_1;
	UartHandle.Init.Parity     = UART_PARITY_NONE;
	UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode       = UART_MODE_TX_RX;

	if (HAL_UART_Init(&UartHandle) != HAL_OK)
	{
//		Error_Handler();
	}
}
#endif

int trap_handling = 0;
/*
int _read(int file, char *ptr, int len) {
    int n;
    int num = 0;
    switch (file) {
    case STDIN_FILENO:
        for (n = 0; n < len; n++) {
            char c = Usart1Get();
            *ptr++ = c;
            num++;
        }
        break;
    default:
        errno = EBADF;
        return -1;
    }
    return num;
}

int _write(int file, char *ptr, int len) {
    int n;
    switch (file) {
    case STDOUT_FILENO: // stdout
        for (n = 0; n < len; n++) {
            Usart1Put(*ptr++ & (uint16_t)0x01FF);
        }
        break;
    case STDERR_FILENO: // stderr
        for (n = 0; n < len; n++) {
            Usart1Put(*ptr++ & (uint16_t)0x01FF);
        }
        break;
    default:
        errno = EBADF;
        return -1;
    }
    return len;
}
 */
int _write(int file, char *ptr, int len)
{
#if 0
//	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
	if (HAL_UART_Transmit(&huart6, (uint8_t *)ptr, len, 0xFFFF) !=  HAL_OK)

//	if (HAL_UART_Transmit(&UartHandle, (uint8_t*)ptr, len, 5000) !=  HAL_OK)
	{
//		Error_Handler();
	}
#else
	int DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++)
	{
		if (trap_handling)
		{
			PutChar(*ptr++);
		}
		else
		{
			__io_putchar( *ptr++ );
		}
	}
#endif
	return len;
}

int _close(int file)
{
	return -1;
}

int _fstat(int file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty(int file)
{
	return 1;
}

int _lseek(int file, int ptr, int dir)
{
	return 0;
}

int _read(int file, char *ptr, int len)
{
	int DataIdx;

	for (DataIdx = 0; DataIdx < len; DataIdx++)
	{
	  *ptr++ = __io_getchar();
	}

   return len;
}

int _open(char *path, int flags, ...)
{
	/* Pretend like we always fail */
	return -1;
}

int _wait(int *status)
{
	errno = ECHILD;
	return -1;
}

int _unlink(char *name)
{
	errno = ENOENT;
	return -1;
}

int _times(struct tms *buf)
{
	return -1;
}

int _stat(char *file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _link(char *old, char *new)
{
	errno = EMLINK;
	return -1;
}

int _fork(void)
{
	errno = EAGAIN;
	return -1;
}

int _execve(char *name, char **argv, char **env)
{
	errno = ENOMEM;
	return -1;
}


//void _init(void)
//{
//}

#if 0
/**
  * @brief  Update FIFO configuration
  * @param  hpcd: PCD handle
  * @retval status
  */
HAL_StatusTypeDef HAL_PCDEx_SetTxFiFo(PCD_HandleTypeDef *hpcd, uint8_t fifo, uint16_t size)
{
  uint8_t i = 0;
  uint32_t Tx_Offset = 0;


  /*  TXn min size = 16 words. (n  : Transmit FIFO index)
  *   When a TxFIFO is not used, the Configuration should be as follows:
  *       case 1 :  n > m    and Txn is not used    (n,m  : Transmit FIFO indexes)
  *       --> Txm can use the space allocated for Txn.
  *       case2  :  n < m    and Txn is not used    (n,m  : Transmit FIFO indexes)
  *       --> Txn should be configured with the minimum space of 16 words
  *  The FIFO is used optimally when used TxFIFOs are allocated in the top
  *       of the FIFO.Ex: use EP1 and EP2 as IN instead of EP1 and EP3 as IN ones.
  *   When DMA is used 3n * FIFO locations should be reserved for internal DMA registers */

  Tx_Offset = hpcd->Instance->GRXFSIZ;

  if(fifo == 0)
  {
    hpcd->Instance->DIEPTXF0_HNPTXFSIZ = (size << 16) | Tx_Offset;
  }
  else
  {
    Tx_Offset += (hpcd->Instance->DIEPTXF0_HNPTXFSIZ) >> 16;
    for (i = 0; i < (fifo - 1); i++)
    {
      Tx_Offset += (hpcd->Instance->DIEPTXF[i] >> 16);
    }

    /* Multiply Tx_Size by 2 to get higher performance */
    hpcd->Instance->DIEPTXF[fifo - 1] = (size << 16) | Tx_Offset;

  }

  return HAL_OK;
}

/**
  * @brief  Update FIFO configuration
  * @param  hpcd: PCD handle
  * @retval status
  */
HAL_StatusTypeDef HAL_PCDEx_SetRxFiFo(PCD_HandleTypeDef *hpcd, uint16_t size)
{

  hpcd->Instance->GRXFSIZ = size;

  return HAL_OK;
}
#endif
