// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009, 2010 MatrixPilot Team
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


#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"

//	main program for testing the IMU.
//	Select device options:

_FOSC( CSW_FSCM_OFF & HS ) ; 	// external high speed crystal
_FWDT( WDT_OFF ) ;				// no watchdog timer
_FBORPOR( 	PBOR_OFF & // brown out detection off
			//PBOR_ON & // brown out detection on
			//BORV_27 & // brown out triggered at 2.7V
			//PWRT_64 & // powerup timer at 64ms
			MCLR_EN &  // enable MCLR
			RST_PWMPIN & // pwm pins as pwm
			PWMxH_ACT_HI & // PWMH is active high
			PWMxL_ACT_HI ) ; // PMWL is active high
_FGS( CODE_PROT_OFF ) ; // no protection
_FICD( 0xC003 ) ;		// normal use of debugging port

void init_all(void)
{
	TRISFbits.TRISF0 = 0 ;
	init_capture() ; 	// initialize capture module
	init_pwm() ;		//  set up the PWM
	init_ADC() ;		//  initialze the AD
	init_GPS2() ;    	//  set up communications to the GPS
	init_clock() ; 		//  turn on the clock
	init_T3()  ;		//  enable the T3 internally triggered interrupt
	init_states() ;
	init_USART1() ;
	setup_USART1_rate() ;
	init_behavior() ;
	init_I2C() ;
	return ;
}

int main (void)
{	//	Initialize all modules and turn on the interrupts.
	defaultCorcon = CORCON;
	init_all() 		;   // initialize interrupt handlers
	SRbits.IPL = 0 ;	// turn on all interrupt priorities
	
	while (1)			//  nothing else to do...entirely interrupt driven
	{
		indicate_loading_main ;
	}
	return 0 ;
}
	
