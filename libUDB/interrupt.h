// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2011 MatrixPilot Team
// See the AUTHORS.TXT file for a list of authors of MatrixPilot.
//
// MatrixPilot is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MatrixPilot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MatrixPilot.  If not, see <http://www.gnu.org/licenses/>.


extern int16_t defaultCorcon ;
extern uint16_t cpu_timer ;
extern uint16_t _cpu_timer ;

void sleep(void);
void idle(void);
uint16_t SP_start(void);
uint16_t SP_limit(void);
uint16_t SP_current(void);

#define T1_INT	0x0001
//#define T2_INT	0x0002
//#define T3_INT	0x0004
//#define T4_INT	0x0008
#define T5_INT	0x0010
#define T6_INT	0x0020
#define T7_INT	0x0040
//#define T8_INT	0x0080

#define IC1_INT	0x0100
#define IC2_INT	0x0200
#define IC3_INT	0x0400
#define IC4_INT	0x0800
#define IC5_INT	0x1000
#define IC6_INT	0x2000
#define IC7_INT	0x4000
#define IC8_INT	0x8000

#define INT1_INT	0x0001
#define INT3_INT	0x0002
#define SPI1_INT	0x0004
#define SPI2_INT	0x0008

#define MI2C1_INT	0x0010
#define MI2C2_INT	0x0020
#define EVTL_INT	0x0040
#define EVTM_INT	0x0080
//#define EVTH_INT

#define U1TX_INT	0x0100
#define U1RX_INT	0x0200
#define U2TX_INT	0x0400
#define U2RX_INT	0x0800

//#define DMA1_INT	0x1000
//#define DMA2_INT	0x2000
#define DMA3_INT	0x4000
#define DMA4_INT	0x8000

#if 1
#define indicate_loading_main	LATAbits.LATA6 = 0
#define indicate_loading_inter	LATAbits.LATA6 = 1
#else
#define indicate_loading_inter	{							\
									T5CONbits.TON = 1 ;		\
								}

#define indicate_loading_main	{							\
									T5CONbits.TON = 0 ;		\
								}
#endif

// The dsp math calls change and restore CORCON internally, so if we fire an
// ISR in the middle of a dsp math call, CORCON can be set to an unexpected value.
// To prevent this, we restore CORCON to the application default when entering
// each ISR, which we saved in main().  Then, when exiting each ISR, we restore
// CORCON to the value it had when we entered.

#ifdef DEBUG_INTERRUPTS

extern volatile uint16_t active_inta ;
extern volatile uint16_t active_intb ;

#define interrupt_save_set_corcon(a, b)		\
	{										\
		__asm__("push CORCON");				\
		CORCON = defaultCorcon;				\
		active_inta |= a;					\
		active_intb |= b;					\
	}

#define interrupt_restore_corcon(a, b)	\
	{										\
		active_inta ^= a;					\
		active_intb ^= b;					\
		__asm__("pop CORCON");				\
	}

#else

#define interrupt_save_set_corcon(a, b)		\
	{										\
		__asm__("push CORCON");				\
		CORCON = defaultCorcon;				\
	}

#define interrupt_restore_corcon(a, b)	\
	{										\
		__asm__("pop CORCON");				\
	}

#endif // DEBUG_INTERRUPTS
