
#include "../libUDB/libUDB.h"
#include "options.h"

#define CONFIRM_TIME ( 20 ) // 10 seconds to confirm cut down
#define CUT_DOWN_PULSE_WIDTH ( 120 ) // 60 seconds pulse

static int16_t cut_down_timer = CONFIRM_TIME ;

static int16_t cut_down_manual_input = 0 ;
static int16_t cut_down_trigger = 0 ;

static void ascent(void) ;
static void confirm(void) ;
static void start_cut_down(void) ;
static void end_cut_down(void) ;
static void wait(void) ;

static void (* cut_down_state) (void) = &ascent ;

void init_cut_down(void)
{
	cut_down_timer = CONFIRM_TIME ;
	cut_down_state = &ascent ;
	_LATA1 = 0 ; // initialize to be off
	_TRISA1 = 0 ; // cut down output is on pin RA1
}

static void ascent(void)
{
	if ( cut_down_trigger == 1)
	{
		cut_down_state = &confirm ;
	}
	else
	{
		cut_down_timer = CONFIRM_TIME ;
	}
}

static void confirm(void)
{
	if ( cut_down_trigger == 1)
	{
		if ( cut_down_timer-- == 0 )
		{
			cut_down_timer = CUT_DOWN_PULSE_WIDTH ;
			cut_down_state = &start_cut_down ;
		}		
	}
	else
	{
		cut_down_timer = CONFIRM_TIME ;
		cut_down_state = &ascent ;
	}
}

static void start_cut_down(void)
{
	LED_BLUE = LED_ON ;
	_LATA1 = 1 ;
	if ( cut_down_timer-- == 0 )
	{
		cut_down_state = &end_cut_down ;
	}
}

static void end_cut_down(void)
{
	LED_BLUE = LED_OFF ;
	_LATA1 = 0 ;
	if ( cut_down_trigger == 0 )
	{
		cut_down_timer = CONFIRM_TIME ;
		cut_down_state = &wait ;
	}
}

static void wait(void)
{
	if ( cut_down_manual_input == 1)
	{
		cut_down_state = &confirm ;
	}
	else
	{
		cut_down_timer = CONFIRM_TIME ;
	}
}

void cut_down_logic(void)
{
	if ( CUT_DOWN_CHANNEL_REVERSED )
	{
		if ( udb_pwIn[ CUT_DOWN_INPUT_CHANNEL ] > 3000 )
		{
			cut_down_manual_input = 0 ;
		}
		else
		{
			cut_down_manual_input = 1 ;
		}
	}
	else
	{
		if ( udb_pwIn[ CUT_DOWN_INPUT_CHANNEL ] > 3000 )
		{
			cut_down_manual_input = 1 ;
		}
		else
		{
			cut_down_manual_input = 0 ;
		}
	}
	if ( cut_down_manual_input || ( udb_flags._.radio_on != 1 ))
	{
		LED_ORANGE = LED_ON ;
		cut_down_trigger = 1 ;
	}
	else
	{
		LED_ORANGE = LED_OFF ;
		cut_down_trigger = 0 ;
	}
	(* cut_down_state) () ; // execute state machine
}
	