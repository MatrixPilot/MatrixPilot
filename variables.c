#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"

//	Variables.


int risec1 , risec2 , risec7 , risec8 ; // rising edge clock capture for radio inputs
int pwc1 , pwc2 , pwc7 , pwc8 ; // pulse widths of radio inputs
int dutycycle ; // used to compute PWM duty cycle

int waggle = 0 ;
int calib_timer, standby_timer ;
int ruddtrim = 3000 , elevtrim = 3000, pulsesselin = 0 , pwrud = 3000 , pwele = 3000 ;
int throttleIdle ;

