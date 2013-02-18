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


#include "libDCM.h"

void dcm_init_rmat(void) ;

void estYawDrift(void) ;
void estimateWind(void) ;

void commit_gps_data(void) ;

void gpsoutline(char message[]) ;
void gpsoutbin(int16_t length , const uint8_t msg[] ) ;

void dcm_run_imu_step(void) ;	// This needs to be run every 25ms
void rxMagnetometer(void) ;		// This should be called every 250ms

void read_accel() ;
void read_gyros() ;

extern uint16_t velocity_previous  ;

extern fractional dirovergndHRmat[] ;
extern fractional dirovergndHGPS[] ;
extern fractional omega[] ;

extern fractional locationErrorEarth[3] ;

#if ( (MAG_YAW_DRIFT == 1) || ( HILSIM == 1 ) )
extern fractional magFieldEarth[3] ;
#endif

extern union longbbbb tow ;
extern union intbb    sog_gps , cog_gps , climb_gps, week_no ;
extern union intbb	  as_sim ;
extern union longbbbb as_sim_ ;
extern uint8_t  hdop ;
extern union longbbbb xpg , ypg , zpg ;
extern union intbb    xvg , yvg , zvg ;
extern uint8_t  mode1 , mode2 , svs, hdop ;

extern int16_t forward_acceleration  ;
extern uint16_t air_speed_3DGPS ;

extern uint8_t  	lat_cir ;
extern int16_t				cos_lat ;

#if ( HILSIM == 1 )
extern union intbb		g_a_x_sim_, g_a_y_sim_, g_a_z_sim_; 
extern union intbb		g_a_x_sim, g_a_y_sim, g_a_z_sim;  
extern union intbb		p_sim_, q_sim_, r_sim_; 
extern union intbb		p_sim, q_sim, r_sim;  
#endif
