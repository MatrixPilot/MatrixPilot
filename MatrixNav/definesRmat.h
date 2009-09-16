
#include "optionsRmat.h"

#include <dsp.h>

struct ADchannel {
 	int input ; // raw input
	int value ; // filtered a little bit as part of A/D
	int offset ;  // baseline at power up 
	 }  // variables for processing an AD channel
 ;

struct ww { int W0 ; int W1 ; } ;
union longww { long  WW ; struct ww _ ; } ;

void init_ADC( void ) ;
void init_pwm( void ) ;
void imu(void) ;

extern struct ADchannel xaccel, yaccel , zaccel ; // x, y, and z accelerometer channels
extern struct ADchannel xrate , yrate, zrate ;  // x, y, and z gyro channels

extern int firstsamp ; // used on startup to detect first A/D sample
extern int calibcount ; // number of PWM pulses before control is turned on

struct relative2D { int x ; int y ; } ;

struct bbbb { unsigned char B0 ; unsigned char B1 ; unsigned char B2 ; unsigned char B3 ; } ;
struct bb { unsigned char B0 ; unsigned char B1 ; } ;

union intbb { int BB ; struct bb _ ; } ;
union longbbbb { long WW ; struct ww _ ; struct bbbb __ ; } ;

void set_gps2(void) ;
void init_T3(void) ;
void init_GPS2(void) ;
void init_USART1(void) ;

int cosine ( signed char angle ) ;
int sine ( signed char angle ) ;
signed char rect_to_polar ( struct relative2D *xy ) ;
void rotate( struct relative2D *xy , signed char angle ) ;
void estYawDrift(void) ;
extern fractional dirovergndHRmat[] ;
extern fractional dirovergndHGPS[] ;
extern fractional rmat[] ;
extern fractional omega[] ;
extern fractional omegaAccum[] ;
extern fractional omegagyro[] ;

extern union longbbbb lat_gps_ , long_gps_ , alt_sl_gps_ ;
extern union intbb    nav_valid_ , nav_type_ , sog_gps_ , cog_gps_ , climb_gps_ ;
extern unsigned char  hdop_ ;
extern union longbbbb xpg_ , ypg_ , zpg_ ;
extern union intbb    xvg_ , yvg_ , zvg_ ;
extern unsigned char  mode1_ , mode2_ , svs_ ;

extern union longbbbb lat_gps , long_gps , alt_sl_gps ;
extern union intbb    nav_valid , nav_type , sog_gps , cog_gps , climb_gps ;
extern unsigned char  hdop ;
extern union longbbbb xpg , ypg , zpg ;
extern union intbb    xvg , yvg , zvg ;
extern unsigned char  mode1 , mode2 , svs ;

extern unsigned char  	lat_cir ;
extern int				cos_lat ;

extern union longbbbb lat_origin , long_origin , alt_origin ;
extern union longbbbb x_origin , y_origin , z_origin ;

struct flag_bits { unsigned int unused 	: 4 ;
			unsigned int yaw_req		: 1 ;
			unsigned int save_origin   	: 1  ;
			unsigned int GPS_steering	: 1  ;
			unsigned int pitch_feedback	: 1  ;
			unsigned int altitude_hold  : 1  ;
			unsigned int use_waypoints	: 1  ;
			unsigned int radio_on		: 1  ;
			unsigned int man_req		: 1  ;
			unsigned int auto_req		: 1  ;
			unsigned int home_req		: 1  ;
			unsigned int nav_capable	: 1  ;
			unsigned int GPS_config		: 1  ;
			 } ;

extern union fbts_int { struct flag_bits _ ; int WW ; } flags ;

extern signed char	desired_dir , actual_dir ;


