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


#include "libDCM_internal.h"
#include <string.h>


struct relative3D GPSlocation 		  = { 0 , 0 , 0 } ;
struct relative3D GPSvelocity 		  = { 0 , 0 , 0 } ;
struct relative2D velocity_thru_air   = { 0 , 0 } ;

union longbbbb lat_gps , long_gps , alt_sl_gps, tow ;  	// latitude, longitude, altitude
union intbb    sog_gps , cog_gps , climb_gps, week_no ;	// speed over ground, course over ground, climb
unsigned char  hdop ;									// horizontal dilution of precision

union longbbbb lat_origin , long_origin , alt_origin ;

union longbbbb xpg , ypg , zpg ;						// gps x, y, z position
union intbb    xvg , yvg , zvg ;						// gps x, y, z velocity 
unsigned char  mode1 , mode2 , svs ;					// gps mode1, mode2, and number of satellites

unsigned char  	lat_cir ;
int				cos_lat = 0 ;

int gps_data_age ;

char *gps_out_buffer = 0 ;
int gps_out_buffer_length = 0 ;
int gps_out_index = 0 ;


extern void (* msg_parse ) ( unsigned char inchar ) ;


void gpsoutbin(int length , const unsigned char msg[] )  // output a binary message to the GPS
{
	gps_out_buffer = 0 ; // clear the buffer pointer first, for safety, in case we're interrupted
	gps_out_index = 0 ;
	gps_out_buffer_length = length ;
	gps_out_buffer = (char*)msg ;
	
	udb_gps_start_sending_data() ;
	
	return ;
}


void gpsoutline(char message[]) // output one NMEA line to the GPS
{
	gpsoutbin(strlen(message), (unsigned char*)message) ;
	return ;
}


int udb_gps_callback_get_byte_to_send(void)
{
	if (gps_out_buffer != 0 && gps_out_index < gps_out_buffer_length) {
		// We have a byte to send
		return (unsigned char)(gps_out_buffer[gps_out_index++]) ;
	}
	else
	{
		// No byte to send, so clear the link to the buffer
		gps_out_buffer = 0 ;
	}
	return -1 ;
}


// Got a character from the GPS
void udb_gps_callback_received_byte(char rxchar)
{
	//bin_out ( rxchar ) ; // binary out to the debugging USART	
	(* msg_parse) ( rxchar ) ; // parse the input byte
	return ;
}


// Received a full set of GPS messages
void udb_background_callback_triggered(void) 
{
	union longbbbb accum_nav ;
	
	if ( gps_nav_valid() )
	{
		commit_gps_data() ;
	}
	
	estYawDrift() ;
	
	if ( gps_nav_valid() )
	{
		gps_data_age = 0 ;
		
		accum_nav.WW = ((lat_gps.WW - lat_origin.WW)/90) ; // in meters, range is about 20 miles
		GPSlocation.y = accum_nav._.W0 ;
		
		//	multiply the longitude delta by the cosine of the latitude
		accum_nav.WW = ((long_gps.WW - long_origin.WW)/90) ; // in meters
		accum_nav.WW = ((__builtin_mulss ( cos_lat , accum_nav._.W0 )<<2)) ;
		GPSlocation.x = accum_nav._.W1 ;
		
		accum_nav.WW = ( alt_sl_gps.WW - alt_origin.WW)/100 ; // height in meters
		GPSlocation.z = accum_nav._.W0 ;
		
		dcm_callback_gps_location_updated() ;
		estimateWind() ;
	}
	else
	{
		gps_data_age = GPS_DATA_MAX_AGE+1 ;
	}
	
	return ;
}
