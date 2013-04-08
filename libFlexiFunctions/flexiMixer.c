#include "..\MatrixPilot\defines.h"
#include "..\MatrixPilotFBW\fbwCntrl.h"
#include "flexifunction_options.h"
#include "flexiFunctionTypes.h"
#include "flexiFunctionRegisters.h"
#include "flexifunctionServices.h"
#include "..\MatrixPilotFBW\fbw_options.h"

#if(USE_INPUT_CONTROL != 1)
 #error("Must have USE_INPUT_CONTROL = 1")
#endif

#if(OUTPUT_CONTROL_GAIN_MUX != 1)
 #error("Must have OUTPUT_CONTROL_GAIN_MUX = 1")
#endif

#if(OUT_CNTRL_AP_MAN_PREMIX != 1)
 #error("Must have OUT_CNTRL_AP_MAN_PREMIX = 1")
#endif

#if(OUTPUT_CONTROL_IN_PWM_UNITS != 0)
 #error("Must have OUTPUT_CONTROL_IN_PWM_UNITS = 0")
#endif

#if(MIXER_OUTPUTS_TO_UDB != 1)
 #error("Must have MIXER_OUTPUTS_TO_UDB = 1")
#endif

#if(DO_SAFE_THROTTLE_MIXING != 1)
 #error("Must have DO_SAFE_THROTTLE_MIXING = 1")
#endif


#define RMAX15 0b0110000000000000	//	1.5 in 2.14 format
#define RMAX   0b0100000000000000	//	1.0 in 2.14 format

// Mixer registers
fractional flexiFunction_registers[FLEXIFUNCTION_MAX_REGS];
//
//
void preMix( void )
{
// Bring RMAX scaled output controls into registers.
	flexiFunction_registers[get_input_register_index_from_directory(VIRTUAL_INPUT_CON_ROLL)] 		= out_cntrls[IN_CNTRL_ROLL];
	flexiFunction_registers[get_input_register_index_from_directory(VIRTUAL_INPUT_CON_PITCH)] 		= out_cntrls[IN_CNTRL_PITCH];
	flexiFunction_registers[get_input_register_index_from_directory(VIRTUAL_INPUT_CON_THROTTLE)] 	= out_cntrls[IN_CNTRL_THROTTLE];
	flexiFunction_registers[get_input_register_index_from_directory(VIRTUAL_INPUT_CON_YAW)] 		= out_cntrls[IN_CNTRL_YAW];
	flexiFunction_registers[get_input_register_index_from_directory(VIRTUAL_INPUT_CON_CAMBER)] 		= out_cntrls[IN_CNTRL_CAMBER];
	flexiFunction_registers[get_input_register_index_from_directory(VIRTUAL_INPUT_CON_BRAKE)] 		= out_cntrls[IN_CNTRL_BRAKE];
	flexiFunction_registers[get_input_register_index_from_directory(VIRTUAL_INPUT_CON_FLAP)] 		= out_cntrls[IN_CNTRL_FLAP];
//
	flexiFunction_registers[get_input_register_index_from_directory(VIRTUAL_INPUT_APCON_WAGGLE)]	= ap_cntrls[AP_CNTRL_WAGGLE];
}


void postMix( void )
{
	mixer_outputs[AILERON_LEFT_OUTPUT_CHANNEL] 	= flexiFunction_registers[get_output_register_index_from_directory(VIRTUAL_OUTPUT_AILERON_L)];
	mixer_outputs[ELEVATOR_OUTPUT_CHANNEL] 		= flexiFunction_registers[get_output_register_index_from_directory(VIRTUAL_OUTPUT_ELEVATOR)];
	mixer_outputs[RUDDER_OUTPUT_CHANNEL] 		= flexiFunction_registers[get_output_register_index_from_directory(VIRTUAL_OUTPUT_RUDDER)];
	mixer_outputs[AILERON_RIGHT_OUTPUT_CHANNEL] = flexiFunction_registers[get_output_register_index_from_directory(VIRTUAL_OUTPUT_AILERON_R)];
	mixer_outputs[FLAP_LEFT_OUTPUT_CHANNEL] 	= flexiFunction_registers[get_output_register_index_from_directory(VIRTUAL_OUTPUT_FLAP_L)];
	mixer_outputs[FLAP_RIGHT_OUTPUT_CHANNEL] 	= flexiFunction_registers[get_output_register_index_from_directory(VIRTUAL_OUTPUT_FLAP_R)];
	mixer_outputs[FLAPMID_LEFT_OUTPUT_CHANNEL] 	= flexiFunction_registers[get_output_register_index_from_directory(VIRTUAL_OUTPUT_FLAPMID_L)];
	mixer_outputs[FLAPMID_RIGHT_OUTPUT_CHANNEL] = flexiFunction_registers[get_output_register_index_from_directory(VIRTUAL_OUTPUT_FLAPMID_R)];
	mixer_outputs[SPOILER_OUTPUT_CHANNEL] 		= flexiFunction_registers[get_output_register_index_from_directory(VIRTUAL_OUTPUT_SPOILER)];

	mixer_outputs[THROTTLE_OUTPUT_CHANNEL] 		= flexiFunction_registers[get_output_register_index_from_directory(VIRTUAL_OUTPUT_THROTTLE)];
}


void servoMix( void )
{
	// Check for a new set of functions
	flexiFunction_commit_buffer_check();

	preMix();

	runFlexiFunctions( );
	
	postMix();
};

