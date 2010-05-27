#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"


//	Variables.

unsigned int rise[MAX_INPUTS+1] ;	// rising edge clock capture for radio inputs
int pwIn[MAX_INPUTS+1] ;	// pulse widths of radio inputs
int pwTrim[MAX_INPUTS+1] ;	// initial pulse widths for trimming
int pwOut[MAX_OUTPUTS+1] ;	// pulse widths for servo outputs

int pitch_control, roll_control, yaw_control, altitude_control ;

int waggle = 0 ;
int calib_timer, standby_timer ;
int pulsesselin = 0 ;
int gps_data_age;

boolean timer_5_on = 0 ;
boolean needSaveExtendedState = 0 ;
int defaultCorcon = 0 ;
