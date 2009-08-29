
#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"

void startS(void) ;
void calibrateS(void) ;
void acquiringS(void) ;
void manualS(void) ;
void autoS(void) ;
void returnS(void) ;
void circlingS(void) ;

//	Implementation of state machine.
//	Examine the state of the radio and GPS and supervisory channel to decide how to control the plane.

void (* stateS ) ( void ) = &startS ;


void init_states(void)
{
	flags.WW = 0 ;
	waggle = 0 ;
	stateS = &startS ;
//	flags._.GPS_config = 1 ;
	return ;
}

void state_machine(void)
{
	//	Configure the GPS for binary if there is a request to do so.
	//	Determine whether the radio is on.
	if ( pulsesselin > 50 )
	{
		flags._.radio_on = 1 ;
		pulsesselin = 0 ;
		LATFbits.LATF1 = 0 ; // indicate radio on
		//	Select manual, automatic, or come home, based on pulse width of channel 4 (pwc1).
		if ( pwc1 > 3500 )
		{
			flags._.man_req = 0 ;
			flags._.auto_req = 0 ;
			flags._.home_req = 1 ;
		}
		else if ( pwc1 > 3000 )
		{
			flags._.man_req = 0 ;
			flags._.auto_req = 1 ;
			flags._.home_req = 0 ;
		}
		else
		{
			flags._.man_req = 1 ;
			flags._.auto_req = 0 ;
			flags._.home_req = 0 ;
		}
	}
	else
	{
		flags._.radio_on = 0 ;
		flags._.man_req = 0 ;
		flags._.auto_req = 0 ;
		flags._.home_req = 0 ;
		LATFbits.LATF1 = 1 ; // indicate radio off
		pulsesselin = 0 ;
	}
	//	Execute the activities for the current state.
	(* stateS) () ;
	//	Reset the nav capable flag. If the GPS has a lock, this flag will be set the next time.
	flags._.nav_capable = 0 ;
	return ;
}

//	Functions that are executed upon first entrance into a state.

//	Calibrate state is used to wait for the filters to settle before recording A/D offsets.
void ent_calibrateS()
{
	flags._.GPS_steering = 0 ;
	flags._.pitch_feedback = 0 ;
	flags._.altitude_hold = 0 ; 
	waggle = 0 ;
	stateS = &calibrateS ;
	calib_timer = CALIB_PAUSE ;
	LATFbits.LATF0 = 0 ; // turn on mode led
	return ;
}

//	Acquire state is used to wait for the GPS to achieve lock.
void ent_acquiringS()
{
//	flags._.GPS_config = 1 ;
	flags._.GPS_steering = 0 ;
	flags._.pitch_feedback = 0 ;
	flags._.altitude_hold = 0 ; 
	waggle = WAGGLE ;
	stateS = &acquiringS ;
	standby_timer = STANDBY_PAUSE ;
	LATFbits.LATF0 = 1 ;
	throttleIdle = throttleFiltered._.W1 = pwc2 ;
	ruddtrim = pwc7 ;
	elevtrim = pwc8 ;
	return ;
}

//	Manual state is used for direct pass-through control from radio to servos.
void ent_manualS()
{
	flags._.GPS_steering = 0 ;
	flags._.pitch_feedback = 0 ;
	flags._.altitude_hold = 0 ; 
	waggle = 0 ;
	LATFbits.LATF0 = 1 ;
	stateS = &manualS ;
	return ;
}

//	Auto state provides augmented control. 
//	Rudder is direct control, elevator has accelerometer and gyro.
void ent_autoS()
{
	flags._.GPS_steering = 0 ;
	flags._.pitch_feedback = 1 ;
	flags._.altitude_hold = 1 ; 
	waggle = 0 ;
	LATFbits.LATF0 = 0 ;
	stateS = &autoS ;
	return ;
}

//	Come home state, entered when the radio signal is lost.
void ent_returnS()
{
	flags._.GPS_steering = 1 ;
	flags._.pitch_feedback = 1 ;
	flags._.altitude_hold = 0 ; 
	waggle = 0 ;
	LATFbits.LATF0 = 0 ;
	stateS = &returnS ;
	return ;
}

//	Same as the come home state, except the radio is on.
//	Come home is commanded by channel 4.
void ent_circlingS()
{
	flags._.GPS_steering = 1 ;
	flags._.pitch_feedback = 1 ;
	flags._.altitude_hold = 1 ; 
	waggle = 0 ;
	LATFbits.LATF0 = 0 ;
	stateS = &circlingS ;
	return ;
}

void startS(void)
{
	ent_calibrateS() ;
	return ;
}

void calibrateS(void)
{
	if(flags._.radio_on)
	{
		__builtin_btg( &LATF , 0 ) ;
		calib_timer--;
		if (calib_timer>0) return ;
		else ent_acquiringS() ;
	}
	else
	{
		ent_calibrateS() ;
	}
	return ;
}

void acquiringS(void)
{
	if(flags._.nav_capable)
	{
		if(flags._.radio_on)
		{
			waggle = - waggle ;
			standby_timer-- ;
			if ( standby_timer>0) return ;
			else
			{
				flags._.save_origin = 1 ;
				ent_manualS() ;
			}
		}
		else
		{
			ent_calibrateS() ;
		}
	}
	else
	{
		ent_acquiringS() ;
	}
	return ;
}

void manualS(void) 
{
	if(flags._.radio_on)
	{
		if ( flags._.home_req & flags._.nav_capable )
		{
			ent_circlingS() ;
		}
		else if( flags._.auto_req ) 
		{
			ent_autoS() ;
		}
	}
	else
	{
		ent_returnS() ;
	}
	return ;
}


void autoS(void) 
{
	if(flags._.radio_on)
	{
		if ( flags._.home_req & flags._.nav_capable )
		{
			ent_circlingS() ;
		}
		else if( flags._.man_req ) ent_manualS() ;
	}
	else
	{
		ent_returnS() ;
	}
	return ;
}

void returnS(void)
{
	if( flags._.radio_on ) ent_manualS() ;
	return ;
}

void circlingS(void)
{
	__builtin_btg( &LATF , 0 ) ;	
	if (flags._.radio_on )
	{
		if(flags._.man_req) ent_manualS() ;
		else if (flags._.auto_req) ent_autoS() ;
	}
	else
	{
		ent_returnS() ;
	}
}

