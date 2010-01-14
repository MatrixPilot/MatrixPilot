#include "p30f4011.h"
#include "defines.h"
#include "definesRmat.h"
#include <stdio.h>
#include <stdarg.h>

int sb_index = 0 ;
int end_index = 0 ;
int telemetry_counter = 5 ;

#define SERIAL_BUFFER_SIZE 256
char serial_buffer[SERIAL_BUFFER_SIZE] ;

void init_USART1(void)
{	
//	debugging/telemetry USART, runs at 19200 baud
	U1MODE = 0b0010000000000000 ; // turn off RX, used to clear errors
	U1STA  = 0b0000010100010000 ;

//Baud Rate = FCY / (16*(BRG+1))
#if ( SERIAL_OUTPUT_FORMAT == SERIAL_OSD_REMZIBI )
	// For the Remzibi OSD, the 'extra' serial port is set to 9600 baud
	U1BRG =  25 ; // 9600 baud
#else
	// Otherwise, the baud rate is set as specified below
//	U1BRG =  51 ; // 4800 baud
//	U1BRG =  25 ; // 9600 baud
	U1BRG =  12 ; // 19200 baud
#endif

	U1MODEbits.UARTEN = 1 ; // turn on uart
	U1MODEbits.ALTIO = 1 ; // alternate pins
	
	U1STAbits.UTXEN = 1 ; // turn on transmitter

	IFS0bits.U1RXIF = 0 ; // clear the interrupt
	IPC2bits.U1RXIP = 3 ; // priority 3
	IEC0bits.U1RXIE = 1 ; // turn on the interrupt

	IFS0bits.U1TXIF = 0 ; // clear the interrupt 
 	IPC2bits.U1TXIP = 4 ; // priority 4 
 	IEC0bits.U1TXIE = 1 ; // turn on the interrupt

	return ;
}

void __attribute__((__interrupt__,__no_auto_psv__)) _U1RXInterrupt(void)
{
	interrupt_save_extended_state ;
	
	indicate_loading_inter ;
	
	char rxchar ;
	indicate_loading_inter ;
	rxchar = U1RXREG ;
	if ( U1STAbits.FERR ) {  init_USART1(); }
	else if ( U1STAbits.OERR ) {  init_USART1(); }
	
	IFS0bits.U1RXIF = 0 ; // clear the interrupt
	
	interrupt_restore_extended_state ;
	return ;
}

// add this text to the output buffer
void serial_output( char* format, ... )
{
	va_list arglist ;
	unsigned char txchar ;
	
	va_start(arglist, format) ;
	
	int start_index = end_index ;
	int remaining = SERIAL_BUFFER_SIZE - start_index ;
	
	if (remaining > 1)
	{
		int wrote = vsnprintf( (char*)(&serial_buffer[start_index]), (size_t)remaining, format, arglist) ;
		end_index = start_index + wrote;
	}
	
	if (sb_index == 0)
	{
		IFS0bits.U1TXIF = 1 ; // trigger the TX interrupt
	}
	
	va_end(arglist);
	
	return ;
}

void __attribute__((__interrupt__,__no_auto_psv__)) _U1TXInterrupt(void)
{
	interrupt_save_extended_state ;
	
	indicate_loading_inter ;
	
	unsigned char txchar ;
	IFS0bits.U1TXIF = 0 ; // clear the interrupt 
	txchar = serial_buffer[ sb_index++ ] ;
	if ( txchar )
	{
		U1TXREG = txchar ;
	}
	else
	{
		sb_index = 0 ;
		end_index = 0 ;
	}
	
	interrupt_restore_extended_state ;
	return ;
}



extern signed char bearing_to_origin ;
extern int tofinish, crosstrack, desiredHeight, waypointIndex, tofinish ;
extern signed char desired_dir_waypoint ;


#if ( SERIAL_OUTPUT_FORMAT == SERIAL_DEBUG )

void serial_output_gps( void )
{
	serial_output("lat: %li, long: %li, alt: %li\r\nrmat: %i, %i, %i, %i, %i, %i, %i, %i, %i\r\n" ,
		lat_gps.WW , long_gps.WW , alt_sl_gps.WW ,
		rmat[0] , rmat[1] , rmat[2] ,
		rmat[3] , rmat[4] , rmat[5] ,
		rmat[6] , rmat[7] , rmat[8]  ) ;
	return ;
}

#elif ( SERIAL_OUTPUT_FORMAT == SERIAL_ARDUSTATION )

#define BYTECIR_TO_DEGREE 92160		// (360.0/256 * 2^16)
int skip = 0 ;

void serial_output_gps( void )
{
	unsigned int mode ;
	struct relative2D matrix_accum ;
	
	long earth_pitch ;		// pitch in binary angles ( 0-255 is 360 degreres)
	long earth_roll ;		// roll of the plane with respect to earth frame
	//long earth_yaw ;		// yaw with respect to earth frame
	
	if (flags._.GPS_steering == 0 && flags._.pitch_feedback == 0 && flags._.use_waypoints == 0)
		mode = 1 ;
	else if (flags._.GPS_steering == 0 && flags._.pitch_feedback == 1 && flags._.use_waypoints == 0)
		mode = 2 ;
	else if (flags._.GPS_steering == 1 && flags._.pitch_feedback == 1 && flags._.use_waypoints == 1)
		mode = 3 ;
	else if (flags._.GPS_steering == 1 && flags._.pitch_feedback == 1 && flags._.use_waypoints == 0)
		mode = 0 ;
	else
		mode = 99 ; // Unknown
	
	//  Roll
	//  Earth Frame of Reference
	matrix_accum.x = rmat[8] ;
	matrix_accum.y = rmat[6] ;
	earth_roll = rect_to_polar(&matrix_accum) ;					// binary angle (0 - 256 = 360 degrees)
	earth_roll = (-earth_roll * BYTECIR_TO_DEGREE) >> 16 ;		// switch polarity, convert to -180 - 180 degrees
	
	//  Pitch
	//  Earth Frame of Reference
	//  Note that we are using the matrix_accum.x
	//  left over from previous rect_to_polar in this calculation.
	//  so this Pitch calculation must follow the Roll calculation
	matrix_accum.y = rmat[7] ;
	earth_pitch = rect_to_polar(&matrix_accum) ;				// binary angle (0 - 256 = 360 degrees)
	earth_pitch = (-earth_pitch * BYTECIR_TO_DEGREE) >> 16 ;	// switch polarity, convert to -180 - 180 degrees
	
	// Yaw
	// Earth Frame of Reference
	// Ardustation does not use yaw in degrees
	// matrix_accum.x = rmat[4] ;
	// matrix_accum.y = rmat[1] ;
	// earth_yaw = rect_to_polar(&matrix_accum) ;				// binary angle (0 - 256 = 360 degrees)
	// earth_yaw = (earth_yaw * BYTECIR_TO_DEGREE) >> 16 ;		// switch polarity, convert to -180 - 180 degrees
	
	
	// The Ardupilot GroundStation protocol is mostly documented here:
	//    http://diydrones.com/profiles/blogs/ardupilot-telemetry-protocol
	
	if (++skip < GPS_RATE)
	{
		serial_output("+++THH:%i,RLL:%li,PCH:%li,STT:%i,***\r\n",
			(int)((pwOut[THROTTLE_OUTPUT_CHANNEL] - pwTrim[THROTTLE_OUTPUT_CHANNEL])/20),
			earth_roll, earth_pitch,
			mode
		) ;
	}
	else
	{
		serial_output("!!!LAT:%li,LON:%li,SPD:%.2f,CRT:%.2f,ALT:%li,ALH:%i,CRS:%.2f,BER:%i,WPN:%i,DST:%i,***\r\n"
					  "+++THH:%i,RLL:%li,PCH:%li,STT:%i,***\r\n",
			lat_gps.WW / 10 , long_gps.WW / 10 , (sog_gps.BB / 100.0), (climb_gps.BB / 100.0),
			(alt_sl_gps.WW - alt_origin.WW) / 100, desiredHeight, (float)(cog_gps.BB), bearing_to_origin,
			waypointIndex, tofinish,
			(int)((pwOut[THROTTLE_OUTPUT_CHANNEL] - pwTrim[THROTTLE_OUTPUT_CHANNEL])/20),
			earth_roll, earth_pitch,
			mode
		) ;
		skip = 0 ;
	}
	
	return ;
}

#elif ( SERIAL_OUTPUT_FORMAT == SERIAL_UDB )

int skip = 0;

void serial_output_gps( void )
{
	// Skip all but every N runs through this function.
	if (++skip < GPS_RATE) return ;
	skip = 0 ;
	
	switch (telemetry_counter)
	{
		// The first 5 lines of telemetry data sent contain info about the compile-time settings from the options.h file
		case 5:
			serial_output("F4:R_STAB=%i:P_STAB=%i:Y_STAB_R=%i:Y_STAB_A=%i:AIL_NAV=%i:RUD_NAV=%i:ALT_HOLD=%i:RACE=%i:\r\n",
				ROLL_STABILIZATION, PITCH_STABILIZATION, YAW_STABILIZATION_RUDDER, YAW_STABILIZATION_AILERON,
				AILERON_NAVIGATION, RUDDER_NAVIGATION, USE_ALTITUDEHOLD, RACING_MODE) ;
			break ;
		case 4:
			serial_output("F5:YAWKP_A=%5.3f:YAWKD_A=%5.3f:ROLLKP=%5.3f:ROLLKD=%5.3f:A_BOOST=%3.1f:\r\n",
				YAWKP_AILERON, YAWKD_AILERON, ROLLKP, ROLLKD, AILERON_BOOST ) ;
			break ;
		case 3:
			serial_output("F6:P_GAIN=%5.3f:P_KD=%5.3f:RUD_E_MIX=%5.3f:ROL_E_MIX=%5.3f:E_BOOST=%3.1f:\r\n",
				PITCHGAIN, PITCHKD, RUDDER_ELEV_MIX, ROLL_ELEV_MIX, ELEVATOR_BOOST) ;
			break ;
		case 2:
			serial_output("F7:Y_KP_R=%5.4f:Y_KD_R=%5.3f:RUD_BOOST=%5.3f:RTL_PITCH_DN=%5.3f:\r\n",
				YAWKP_RUDDER, YAWKD_RUDDER, RUDDER_BOOST, RTL_PITCH_DOWN) ;
			break ;
		case 1:
			serial_output("F8:H_MAX=%6.1f:H_MIN=%6.1f:MIN_THR=%3.2f:MAX_THR=%3.2f:PITCH_MIN_THR=%4.1f:PITCH_MAX_THR=%4.1f:PITCH_ZERO_THR=%4.1f:\r\n",
				HEIGHT_TARGET_MAX, HEIGHT_TARGET_MIN, ALT_HOLD_THROTTLE_MIN, ALT_HOLD_THROTTLE_MAX,
				ALT_HOLD_PITCH_MIN, ALT_HOLD_PITCH_MAX, ALT_HOLD_PITCH_HIGH) ;
			break ;
		default:
			// F2 below means "Format Revision 2: and is used by a Telemetry parser to invoke the right pattern matching
			// If you change this output format, then change F2 to F3 or F4, etc - to mark a new revision of format.
			// F2 is a compromise between easy reading of raw data in a file and not droppping chars in transmission.
			serial_output("F2:T%li:S%d%d%d%d:N%li:E%li:A%li:W%i:a%i:b%i:c%i:d%i:e%i:f%i:g%i:h%i:i%i:c%u:s%i:\r\n",
				tow, flags._.radio_on, flags._.nav_capable, flags._.GPS_steering, flags._.use_waypoints,
				lat_gps.WW , long_gps.WW , alt_sl_gps.WW, waypointIndex,
				rmat[0] , rmat[1] , rmat[2] ,
				rmat[3] , rmat[4] , rmat[5] ,
				rmat[6] , rmat[7] , rmat[8] ,
				(unsigned int)cog_gps.BB, sog_gps.BB ) ;
			return ;
	}
	telemetry_counter-- ;
	return ;
}
#elif ( SERIAL_OUTPUT_FORMAT == SERIAL_OSD_REMZIBI )

void serial_output_gps( void )
{
	// TODO: Output interesting information for OSD.
	// But first we'll have to implement a buffer for passthrough characters to avoid
	// output corruption, or generate NMEA ourselves here.
	return ;
}

#else

void serial_output_gps( void )
{
	return ;
}

#endif

