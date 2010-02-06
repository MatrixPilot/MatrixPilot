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
extern struct ADchannel vref ; // reference voltage

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

extern unsigned char  	lat_cir ;
extern int				cos_lat ;

extern union longbbbb lat_origin , long_origin , alt_origin ;
extern union longbbbb x_origin , y_origin , z_origin ;

struct flag_bits {
			unsigned int unused					: 4 ;
			unsigned int yaw_req				: 1 ;
			unsigned int save_origin   			: 1 ;
			unsigned int GPS_steering			: 1 ;
			unsigned int pitch_feedback			: 1 ;
			unsigned int altitude_hold_throttle	: 1 ;
			unsigned int altitude_hold_pitch	: 1 ;
			unsigned int use_waypoints			: 1 ;
			unsigned int radio_on				: 1 ;
			unsigned int man_req				: 1 ;
			unsigned int auto_req				: 1 ;
			unsigned int home_req				: 1 ;
			unsigned int nav_capable			: 1 ;
			} ;

extern union fbts_int { struct flag_bits _ ; int WW ; } flags ;

extern signed char	desired_dir , actual_dir ;


