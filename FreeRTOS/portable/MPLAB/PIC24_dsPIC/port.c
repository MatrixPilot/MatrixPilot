/*
    FreeRTOS V7.5.3 - Copyright (C) 2013 Real Time Engineers Ltd. 
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>! NOTE: The modification to the GPL is included to allow you to distribute
    >>! a combined work that includes FreeRTOS without being obliged to provide
    >>! the source code for proprietary components outside of the FreeRTOS
    >>! kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
	Changes from V4.2.1

	+ Introduced the configKERNEL_INTERRUPT_PRIORITY definition.
*/

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the PIC24 port.
 *----------------------------------------------------------*/

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "string.h"

/* Hardware specifics. */
#ifdef __XC16__
#include "xc.h"
#elif defined( __PIC24E__ )
#include <p24Exxxx.h>
#elif defined( __PIC24F__ )
#include <p24Fxxxx.h>
#elif defined( __PIC24H__ )
#include <p24Hxxxx.h>
#elif defined( __dsPIC30F__ )
#include <p30Fxxxx.h>
#elif defined( __dsPIC33E__ )
#include <p33Exxxx.h>
#elif defined( __dsPIC33F__ )
#include <p33Fxxxx.h>
#else
#error unknown processor family
#endif

#define portBIT_SET 1
#define portTIMER_PRESCALE 8
#define portINITIAL_SR	0

/* Defined for backward compatability with project created prior to
FreeRTOS.org V4.3.0. */
#ifndef configKERNEL_INTERRUPT_PRIORITY
	#define configKERNEL_INTERRUPT_PRIORITY 1
#endif

/* Use _T1Interrupt as the interrupt handler name if the application writer has
not provided their own. */
#ifndef configTICK_INTERRUPT_HANDLER
	#define configTICK_INTERRUPT_HANDLER _T1Interrupt
#endif /* configTICK_INTERRUPT_HANDLER */

/* The program counter is only 23 bits. */
#define portUNUSED_PR_BITS	0x7f

/* Records the nesting depth of calls to portENTER_CRITICAL(). */
unsigned portBASE_TYPE uxCriticalNesting = 0xef;

//#if configKERNEL_INTERRUPT_PRIORITY != 1
//	#error If configKERNEL_INTERRUPT_PRIORITY is not 1 then the #32 in the following macros needs changing to equal the portINTERRUPT_BITS value, which is ( configKERNEL_INTERRUPT_PRIORITY << 5 )
//#endif
#define STRINGIFY(x)	#x
#define STRINGIFY_E(x)	STRINGIFY(x)
/* NB. restoring of SPLIM requires the stack to be in the lower 32KB of address space
 * this isn't really an imposition as all heck breaks loose if the the stack is in the upper address space - though legal
 */

#define portSAVE_CONTEXTstart																				\
		"PUSH	SR							\n"		/* Save the SR used by the task.... */					\
		"PUSH	W0							\n"		/* ....then disable interrupts. */						\
		"MOV #" STRINGIFY_E(configKERNEL_INTERRUPT_PRIORITY) " << 5, W0\n"									\
		"MOV	W0, SR						\n"																\
		"PUSH	W1							\n"		/* Save registers to the stack. */						\
		"PUSH.D	W2							\n"																\
		"PUSH.D	W4							\n"																\
		"PUSH.D	W6							\n"																\
		"PUSH.D W8							\n"																\
		"PUSH.D W10							\n"																\
		"PUSH.D	W12							\n"																\
		"PUSH	W14							\n"																\
		"PUSH	RCOUNT						\n"																\
		"PUSH	TBLPAG						\n"																\

#define portSAVE_CONTEXTdsp																					\
		"PUSH	ACCAL						\n"																\
		"PUSH	ACCAH						\n"																\
		"PUSH	ACCAU						\n"																\
		"PUSH	ACCBL						\n"																\
		"PUSH	ACCBH						\n"																\
		"PUSH	ACCBU						\n"																\

#define portSAVE_CONTEXTdo																					\
		"PUSH	DCOUNT						\n"																\
		"PUSH	DOSTARTL					\n"																\
		"PUSH	DOSTARTH					\n"																\
		"PUSH	DOENDL						\n"																\
		"PUSH	DOENDH						\n"																\

#ifdef __HAS_EDS__
	#define portSAVE_CONTEXTmem																				\
		"PUSH	CORCON						\n"																\
		"PUSH	DSRPAG						\n"																\
		"PUSH	DSWPAG						\n"																\

#else
	#define portSAVE_CONTEXTmem                                                                             \
		"PUSH	CORCON						\n"																\
		"PUSH	PSVPAG						\n"																\

#endif

#define portSAVE_CONTEXTend																			\
		"MOV	_uxCriticalNesting, W0		\n"		/* Save the critical nesting counter for the task. */	\
		"PUSH	W0							\n"																\
		"PUSH	SPLIM						\n"																\
		"MOV	_pxCurrentTCB, W0			\n"		/* Save the new top of stack into the TCB. */			\
		"MOV	W15, [W0]					\n"

#define portRESTORE_CONTEXTend																				\
		"MOV	_pxCurrentTCB, W0		\n"	/* Restore the stack pointer for the task. */					\
		"MOV	[W0], W1				\n"																	\
		"MOV	[W1], W1				\n"																	\
		"MOV	W1, SPLIM				\n"																	\
		"MOV	[W0], W15				\n"																	\
		"POP	W0						\n"	/* SPLIM - but already set */									\
		"POP	W0						\n"	/* Restore the critical nesting counter for the task. */		\
		"MOV	W0, _uxCriticalNesting	\n"																	\

#ifdef __HAS_EDS__
	#define portRESTORE_CONTEXTmem																			\
		"POP	DSWPAG					\n"																	\
		"POP    DSRPAG					\n"																	\
		"POP	CORCON					\n"																	\

#else
	#define portRESTORE_CONTEXTmem																			\
		"POP	PSVPAG					\n"																	\
		"POP	CORCON					\n"																	\

#endif

#define portRESTORE_CONTEXTdo																				\
		"POP	DOENDH					\n"																	\
		"POP	DOENDL					\n"																	\
		"POP	DOSTARTH				\n"																	\
		"POP	DOSTARTL				\n"																	\
		"POP	DCOUNT					\n"																	\

#define portRESTORE_CONTEXTdsp																				\
		"POP	ACCBU					\n"																	\
		"POP	ACCBH					\n"																	\
		"POP	ACCBL					\n"																	\
		"POP	ACCAU					\n"																	\
		"POP	ACCAH					\n"																	\
		"POP	ACCAL					\n"																	\

#define portRESTORE_CONTEXTstart																			\
		"POP	TBLPAG					\n"																	\
		"POP	RCOUNT					\n"	/* Restore the registers from the stack. */						\
		"POP	W14						\n"																	\
		"POP.D	W12						\n"																	\
		"POP.D	W10						\n"																	\
		"POP.D	W8						\n"																	\
		"POP.D	W6						\n"																	\
		"POP.D	W4						\n"																	\
		"POP.D	W2						\n"																	\
		"POP.D	W0						\n"																	\
		"POP	SR						\n"

#if defined( __PIC24E__ ) || defined ( __PIC24F__ ) || defined( __PIC24FK__ ) || defined( __PIC24H__ )
	#define portRESTORE_CONTEXT 	portRESTORE_CONTEXTmem 												 	portRESTORE_CONTEXTstart
	#define portSAVE_CONTEXT		portSAVE_CONTEXTstart     												portSAVE_CONTEXTmem
#elif defined( __dsPIC30F__ ) || defined( __dsPIC33F__ )
	#define portRESTORE_CONTEXT 	portRESTORE_CONTEXTmem  portRESTORE_CONTEXTdo  	portRESTORE_CONTEXTdsp  portRESTORE_CONTEXTstart
	#define portSAVE_CONTEXT		portSAVE_CONTEXTstart   portSAVE_CONTEXTdsp     portSAVE_CONTEXTdo	 	portSAVE_CONTEXTmem
#elif defined( __dsPIC33E__ )
	#define portRESTORE_CONTEXT 	portRESTORE_CONTEXTmem 					 	 	portRESTORE_CONTEXTdsp  portRESTORE_CONTEXTstart
	#define portSAVE_CONTEXT		portSAVE_CONTEXTstart   portSAVE_CONTEXTdsp     					 	portSAVE_CONTEXTmem
#else
	#error Unrecognised device selected
#endif

void vPortYieldfake() {		/* this skips the standard C pro/epilogue i.e. frame pointer (which can be optimized out */
	asm volatile( "	.global _vPortYield" );
	asm volatile( "_vPortYield:" );
	asm volatile( portSAVE_CONTEXT );
	asm volatile( portSAVE_CONTEXTend );
	vTaskSwitchContext();
	asm volatile( "restoreContext:" );
	asm volatile( portRESTORE_CONTEXTend );
	asm volatile( portRESTORE_CONTEXT );
	asm volatile( "return" );
}

/*
 * Setup the timer used to generate the tick interrupt.
 */
void vApplicationSetupTickTimerInterrupt( void );

/*
 * See header file for description.
 */

#if 1
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
	unsigned short  stack_save2, stack_save3;
	/* Setup the stack as if a yield had occurred.

	Save the low bytes of the program counter. */
	*pxTopOfStack++ = ( portSTACK_TYPE ) pxCode;
	/* Save the high byte of the program counter.  This will always be zero
	here as it is passed in a 16bit pointer.  If the address is greater than
	16 bits then the pointer will point to a jump table. */
	*pxTopOfStack++ = 0;

	/* the following is a little awkward, but has the advantage that it mimics vPortYield */
	asm volatile( "MOV W15,%0" :"=r" (stack_save2): );
	asm volatile( "MOV %0,W0" : :"g" (pvParameters) ); /* pretend we're passing pvParameters */
	asm volatile( portSAVE_CONTEXT );
	asm volatile( "MOV W15,%0" :"=r" (stack_save3):);
	memcpy( pxTopOfStack, (char *)stack_save2, stack_save3-stack_save2 );
//	asm volatile( "MOV %0,W15" : :"r" (stack_save2): );	/* pop off the context stack frame */
	*pxTopOfStack = ( portSTACK_TYPE ) 0;					/* clear the SR register  */

	pxTopOfStack += ( stack_save3-stack_save2 ) / sizeof( portSTACK_TYPE );
	/* Finally the critical nesting depth. */
	*pxTopOfStack++ = 0x00;
	/* the following is done by the macro traceTASK_CREATE */
//	*pxTopOfStack++ = ( portSTACK_TYPE )pxEndOfStack-16;			/* SPLIM - enough space for the exception handler */
	return pxTopOfStack;
}


#else
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
unsigned short usCode;
unsigned portBASE_TYPE i;

const portSTACK_TYPE xInitialStack[] =
{
	0x1111,	/* W1 */
	0x2222, /* W2 */
	0x3333, /* W3 */
	0x4444, /* W4 */
	0x5555, /* W5 */
	0x6666, /* W6 */
	0x7777, /* W7 */
	0x8888, /* W8 */
	0x9999, /* W9 */
	0xaaaa, /* W10 */
	0xbbbb, /* W11 */
	0xcccc, /* W12 */
	0xdddd, /* W13 */
	0xeeee, /* W14 */
	0xcdce, /* RCOUNT */
	0xabac, /* TBLPAG */

	/* dsPIC specific registers. */
	#ifdef __HAS_DSP__
		0x0202, /* ACCAL */
		0x0303, /* ACCAH */
		0x0404, /* ACCAU */
		0x0505, /* ACCBL */
		0x0606, /* ACCBH */
		0x0707, /* ACCBU */
		#ifndef __dsPIC33E__
		0x0808, /* DCOUNT */
		0x090a, /* DOSTARTL */
		0x1010, /* DOSTARTH */
		0x1110, /* DOENDL */
		0x1212, /* DOENDH */
		#endif
	#endif
};

	/* Setup the stack as if a yield had occurred.

	Save the low bytes of the program counter. */
	usCode = ( unsigned short ) pxCode;
	*pxTopOfStack = ( portSTACK_TYPE ) usCode;
	pxTopOfStack++;

	/* Save the high byte of the program counter.  This will always be zero
	here as it is passed in a 16bit pointer.  If the address is greater than
	16 bits then the pointer will point to a jump table. */
	*pxTopOfStack = ( portSTACK_TYPE ) 0;
	pxTopOfStack++;

	/* Status register with interrupts enabled. */
	*pxTopOfStack = portINITIAL_SR;
	pxTopOfStack++;

	/* Parameters are passed in W0. */
	*pxTopOfStack = ( portSTACK_TYPE ) pvParameters;
	pxTopOfStack++;

	for( i = 0; i < ( sizeof( xInitialStack ) / sizeof( portSTACK_TYPE ) ); i++ )
	{
		*pxTopOfStack = xInitialStack[ i ];
		pxTopOfStack++;
	}

	*pxTopOfStack = CORCON;
	pxTopOfStack++;

	#if defined(__HAS_EDS__)
		*pxTopOfStack = DSRPAG;
		pxTopOfStack++;
		*pxTopOfStack = DSWPAG;
		pxTopOfStack++;
	#else /* __HAS_EDS__ */
		*pxTopOfStack = PSVPAG;
		pxTopOfStack++;
	#endif /* __HAS_EDS__ */

	/* Finally the critical nesting depth. */
	*pxTopOfStack = 0x00;
	pxTopOfStack++;

	return pxTopOfStack;
}
#endif
/*-----------------------------------------------------------*/

portBASE_TYPE xPortStartScheduler( void )
{
	/* Setup a timer for the tick ISR. */
	vApplicationSetupTickTimerInterrupt();

	/* Restore the context of the first task to run.
	   big cheat - jump into the tail end of vPortYield */
	asm volatile( "goto restoreContext " );

	/* Should not reach here. */
	return pdTRUE;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* It is unlikely that the scheduler for the PIC port will get stopped
	once running.  If required disable the tick interrupt here, then return
	to xPortStartScheduler(). */
}
/*-----------------------------------------------------------*/

/*
 * Setup a timer for a regular tick.
 */
__attribute__(( weak )) void vApplicationSetupTickTimerInterrupt( void )
{
const unsigned long ulCompareMatch = ( ( configCPU_CLOCK_HZ / portTIMER_PRESCALE ) / configTICK_RATE_HZ ) - 1;

	/* Prescale of 8. */
	T1CON = 0;
	TMR1 = 0;

	PR1 = ( unsigned short ) ulCompareMatch;

	/* Setup timer 1 interrupt priority. */
	IPC0bits.T1IP = configKERNEL_INTERRUPT_PRIORITY;

	/* Clear the interrupt as a starting condition. */
	IFS0bits.T1IF = 0;

	/* Enable the interrupt. */
	IEC0bits.T1IE = 1;

	/* Setup the prescale value. */
	T1CONbits.TCKPS0 = 1;
	T1CONbits.TCKPS1 = 0;

	/* Start the timer. */
	T1CONbits.TON = 1;
}
/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
	portDISABLE_INTERRUPTS();
	uxCriticalNesting++;
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
	uxCriticalNesting--;
	if( uxCriticalNesting == 0 )
	{
		portENABLE_INTERRUPTS();
	}
}
/*-----------------------------------------------------------*/

#ifdef USE_FREERTOS
void __attribute__((__interrupt__, auto_psv)) configTICK_INTERRUPT_HANDLER( void )
{
	/* Clear the timer interrupt. */
	IFS0bits.T1IF = 0;

	if( xTaskIncrementTick() != pdFALSE )
	{
		portYIELD();
	}
}
#endif // USE_FREERTOS

