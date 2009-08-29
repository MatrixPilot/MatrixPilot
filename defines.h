#include "p30f4011.h"
#include "options.h"
#include "controlGains.h"

void init_clock(void) ;
void set_gps2(void) ;
void init_T3(void) ;
void init_ADC( void ) ;
void init_pwm( void ) ;
void init_capture(void) ;
void init_GPS2(void) ;
void init_USART1(void) ;
void init_states(void) ;

int cosine ( signed char angle ) ;
int sine ( signed char angle ) ;

void navigate(void) ;

void state_machine(void) ;
void filterInputs(void) ;
void rudderCntrl(void) ;
void elevatorCntrl(void) ;

void set_baud(void) ;
void set_bin(void) ;

int pulsesat(long) ;

extern struct ADchannel xaccel, yaccel , zaccel ; // x, y, and z accelerometer channels
extern struct ADchannel xrate , yrate, zrate ;  // x, y, and z gyro channels
extern struct ADchannel vref ; // reference voltage

extern int risec1 , risec2 , risec7 , risec8 ; // rising edge clock capture for radio inputs
extern int pwc1 , pwc2 , pwc7 , pwc8 ; // pulse widths of radio inputs
extern int dutycycle ; // used to compute PWM duty cycle
extern int firstsamp ; // used on startup to detect first A/D sample
extern int calibcount ; // number of PWM pulses before control is turned on

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

extern signed char	desired_dir , actual_dir ;

extern int height ;

extern int waggle, calib_timer, standby_timer, ruddtrim, elevtrim, pulsesselin, pwrud, pwele ;
extern int throttleIdle ;
extern union longww throttleFiltered ;
extern int pitchAltitudeAdjust ;

extern int rudderDeflection ;

extern signed char GPS_pitch  ;

extern int velocity_magnitude ;
extern int forward_acceleration  ;
extern int velocity_previous  ;




