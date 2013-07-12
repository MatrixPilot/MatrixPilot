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


#define INT_PRI_T1      6   // background.c : high priority HEARTBEAT_HZ heartbeat of libUDB
//#define INT_PRI_T2      ?   // radioIn.c : does not use the timer interrupt
//#define INT_PRI_T3      ?   // unused
#define INT_PRI_T4      7   // servoOut.c : highest priority interrupt for pwm
#define INT_PRI_T5      6   // background.c : high priority, but ISR is very short - used to measure CPU usage.
#define INT_PRI_T6      3   // background.c : trigger from the high priority heartbeat ISR to start all the HEARTBEAT_HZ processing at a lower priority - NOTE: timer 6 is not actually being used
#define INT_PRI_T7      2   // background.c : used to trigger background tasks such as navigation processing after binary data is received from the GPS

#define INT_PRI_MPUSPI  6   // mpu6000.c : SPI1 (UDB4 or AUAV3) or SPI2 (UDB4 or UDB5)
#define INT_PRI_INT1    6   // mpu6000.c : SPI1 uses external interrupt 1
#define INT_PRI_INT3    6   // mpu6000.c : SPI3 uses external interrupt 3

#define INT_PRI_IC      6   // radioIn.c : input capture interrupt

#define INT_PRI_I2C1    5   // I2C1.c
#define INT_PRI_I2C2    5   // I2C2.c

#define INT_PRI_AD1     5   // analog2digital_xxx.c
#define INT_PRI_DMA0    5   // analog2digital_xxx.c
#define INT_PRI_DMA1    5   // AT45D_DMA.c
#define INT_PRI_DMA2    5   // AT45D_DMA.c

#define INT_PRI_U1TX    4   // serialIO.c : mid range priority, no urgent reason
#define INT_PRI_U1RX    4   // serialIO.c : mid range priority, no urgent reason
#define INT_PRI_U2TX    4   // serialIO.c : mid range priority, no urgent reason
#define INT_PRI_U2RX    4   // serialIO.c : mid range priority, no urgent reason

#define INT_PRI_USB1    4   // USB available only on AUAV3

#define INT_PRI_EVENTM  2   // events.c : trigger M uses the C1 interrupt
#define INT_PRI_EVENTL  1   // events.c : trigger L uses the C2 interrupt


extern int16_t defaultCorcon;
extern uint16_t cpu_timer;
extern uint16_t _cpu_timer;

void sleep(void);
void idle(void);
uint16_t SP_start(void);
uint16_t SP_limit(void);
uint16_t SP_current(void);

#if (USE_MCU_IDLE == 1)
#define indicate_loading_inter {}
#define indicate_loading_main  {}
#else
#define indicate_loading_inter \
	{ \
		T5CONbits.TON = 1; \
	}

#define indicate_loading_main \
	{ \
		T5CONbits.TON = 0; \
	}
#endif // USE_MCU_IDLE

// The dsp math calls change and restore CORCON internally, so if we fire an
// ISR in the middle of a dsp math call, CORCON can be set to an unexpected value.
// To prevent this, we restore CORCON to the application default when entering
// each ISR, which we saved in main().  Then, when exiting each ISR, we restore
// CORCON to the value it had when we entered.

#ifdef __PIC32MX__
#define interrupt_save_set_corcon
#define interrupt_restore_corcon
#else
#define interrupt_save_set_corcon \
	{ \
		__asm__("push CORCON"); \
		CORCON = defaultCorcon; \
	}

#define interrupt_restore_corcon \
	{ \
		__asm__("pop CORCON"); \
	}
#endif // __PIC32MX__
