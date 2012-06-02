// This file is part of the MatrixPilot RollPitchYaw demo.
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


// main program for testing the IMU.


#include "../libDCM/libDCM.h"


// Used for serial debug output
#include "stdio.h"

char debug_buffer[128] ;
int db_index = 0 ;
void send_debug_line( void ) ;

// trap handler variables
// not used at the moment, but the trap handler is needed for clock "false alarms"

volatile int trap_flags __attribute__ ((persistent));
volatile long trap_source __attribute__ ((persistent));
volatile int osc_fail_count __attribute__ ((persistent));

int main (void)
{
	// Set up the libraries
	udb_init() ;
	dcm_init() ;
	
	udb_serial_set_rate(57600) ;
	
	LED_GREEN = LED_OFF ;
	
	// Start it up!
	udb_run() ;  // This never returns.
	
	return 0 ;
}


// Called every 1/2 second at high priority
void udb_background_callback_periodic(void)
{
	if (!dcm_flags._.calib_finished)
	{
		// If still calibrating, blink RED
		udb_led_toggle(LED_GREEN) ;
	}
	else
	{
		// No longer calibrating: solid RED and send debug output
		LED_GREEN = LED_OFF ;
	}
	
	return ;
}


// Called every time we get gps data (1, 2, or 4 Hz, depending on GPS config)
void dcm_callback_gps_location_updated(void)
{
	// Blink GREEN led to show that the GPS is communicating
	udb_led_toggle(LED_GREEN) ;
	return ;
}


// Called at 40 Hz, before sending servo pulses
void dcm_servo_callback_prepare_outputs(void)
{
	if (!dcm_flags._.calib_finished)
	{
		udb_pwOut[ROLL_OUTPUT_CHANNEL] = SERVOCENTER ;
		udb_pwOut[PITCH_OUTPUT_CHANNEL] = SERVOCENTER ;
		udb_pwOut[3] = SERVOCENTER ;		
		udb_pwOut[YAW_OUTPUT_CHANNEL] = SERVOCENTER ;
		udb_pwOut[5] = SERVOCENTER ;
	}
	else
	{
		union longww accum ;
		
		accum.WW = __builtin_mulss( rmat[6] , 10000 ) ;
		udb_pwOut[ROLL_OUTPUT_CHANNEL] = udb_servo_pulsesat(SERVOCENTER + accum._.W1) ;
		
		accum.WW = __builtin_mulss( rmat[7] , 10000 ) ;
		udb_pwOut[PITCH_OUTPUT_CHANNEL] = udb_servo_pulsesat(SERVOCENTER + accum._.W1) ;
		
		accum.WW = __builtin_mulss( rmat[4] , 10000 ) ;
		udb_pwOut[YAW_OUTPUT_CHANNEL] = udb_servo_pulsesat(SERVOCENTER + accum._.W1) ;
	}
	
	// Serial output at 2Hz  (40Hz / 20)
	if (udb_heartbeat_counter % 20 == 0)
	{
		if (dcm_flags._.calib_finished)
		{
			send_debug_line() ;
		}
	}
	
	return ;
}


// Prepare a line of serial output and start it sending
void send_debug_line( void )
{
//	extern unsigned char magreg[6] ;
//	extern unsigned char accreg[6] ;
//	extern int magFieldRaw[3] ;
//	extern int accMeasureRaw[3];
//	extern int gyroMeasureRaw[3];
//	extern int udb_magFieldBody[3] ; 
//	extern int magMessage;
//	extern int I2C1_ERROR;
//	extern int I2C1MAXQ;
//	extern int I2C1MAXS;
//	extern union longbbbb lat_gps , long_gps , time_gps_, date_gps_, alt_sl_gps ;
//	extern union intbb    sog_gps , cog_gps ;
//	extern unsigned char data_valid_ , NS , EW , svs , hdop ;
//	extern unsigned int rmc_counter;
//	extern unsigned char XOR;
//	static unsigned int i = 0;
	db_index = 0 ;
	
	sprintf( debug_buffer , "lat: %li, long: %li, alt: %li\r\nrmat:\t %i, %i, %i,\t %i,\t %i,\t %i,\t %i,\t %i,\t %i\r\n" , 
		lat_gps.WW , long_gps.WW , alt_sl_gps.WW , 
		rmat[0] , rmat[1] , rmat[2] , 
		rmat[3] , rmat[4] , rmat[5] , 
		rmat[6] , rmat[7] , rmat[8]  ) ; 

/*	sprintf( debug_buffer , "%u rmat:  %i, %i, %i,  %i, %i, %i,  %i, %i, %i\r\n" , i++,
		rmat[0] , rmat[1] , rmat[2] , 
		rmat[3] , rmat[4] , rmat[5] , 
		rmat[6] , rmat[7] , rmat[8]  ) ; */

//	sprintf( debug_buffer , "mag raw:%u, %i, %i\t\t %i, %i\t\t %i, %i\r\n",i++, magreg[0], magreg[1], magreg[2], magreg[3], magreg[4], magreg[5] ) ;	
//	sprintf( debug_buffer , "acc raw:%u, %X, %X\t\t %X, %X\t\t %X, %X\r\n",i++, accreg[0], accreg[1], accreg[2], accreg[3], accreg[4], accreg[5] ) ;	
//	sprintf( debug_buffer , "magFieldRaw:%u, %i, %i, %i\r\n",i++, magFieldRaw[0], magFieldRaw[1], magFieldRaw[2] ) ;	
//	sprintf( debug_buffer , "accMeasureRaw:%u, %i, %i, %i\r\n",i++, accMeasureRaw[0], accMeasureRaw[1], accMeasureRaw[2] ) ;	
//	sprintf( debug_buffer , "gyroMeasureRaw:%u, %i, %i, %i\r\n",i++, gyroMeasureRaw[0], gyroMeasureRaw[1], gyroMeasureRaw[2] ) ;	
//	sprintf( debug_buffer , "gyro-acc-mag:%u, %i, %i, %i\t %i, %i, %i\t %i, %i, %i\r\n",i++, gyroMeasureRaw[0], gyroMeasureRaw[1], gyroMeasureRaw[2], accMeasureRaw[0], accMeasureRaw[1], accMeasureRaw[2], magFieldRaw[0], magFieldRaw[1], magFieldRaw[2] ) ;	

//	sprintf( debug_buffer , "%u\tI2C1MAXQ:%d\tI2C1MAXS:%d\tI2C1CON: %X\tI2C1STAT: %X\r\n",i++, I2C1MAXQ, I2C1MAXS, I2C1CON, I2C1STAT ) ;	
//	sprintf( debug_buffer , "magFieldBody:%u, %i, %i, %i\r\n",i++, udb_magFieldBody[0], udb_magFieldBody[1], udb_magFieldBody[2] ) ;	
//	sprintf( debug_buffer , "mag message:%u, %i \r\n",i++, magMessage ) ;	

//	sprintf( debug_buffer , "%u,lat:%li,long:%li\r\ntime:%lu,v:%c,svs:%u,hdop:%u,MSL:%li,sog:%i,cog:%i,dat:%lu\r\n",
//							i++, lat_gps.WW , long_gps.WW, 
//							time_gps_.WW, data_valid_, svs, hdop, alt_sl_gps.WW, sog_gps.BB , cog_gps.BB, date_gps_.WW) ;	
//	sprintf( debug_buffer , "%u,t:%lu,v:%c,svs:%u,hdop:%u,MSL:%li,sog:%i,cog:%i,dat:%lu\r\n",i++, time_gps_.WW, data_valid_, svs, hdop, alt_sl_gps.WW, sog_gps.BB , cog_gps.BB, date_gps_.WW) ;	
//	sprintf( debug_buffer , "%u\r\n",rmc_counter) ;	
//	sprintf( debug_buffer , "%c",ch) ;	
//	sprintf( debug_buffer , "%u",ch) ;	
//	sprintf( debug_buffer , "%2X %2X\r\n",ch,XOR) ;	

	udb_serial_start_sending_data() ;
	
	return ;
}


// Return one character at a time, as requested.
// Requests will stop after we send back a -1 end-of-data marker.
int udb_serial_callback_get_byte_to_send(void)
{
	unsigned char c = debug_buffer[ db_index++ ] ;
	
	if (c == 0) return -1 ;
	
	return c ;
}


// Don't respond to serial input
void udb_serial_callback_received_byte(char rxchar)
{
	// Do nothing
	return ;
}


void udb_callback_radio_did_turn_off( void ) {}
