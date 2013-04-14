#ifndef _FLYBYWIRE_C_
#define _FLYBYWIRE_C_

#include "defines.h"
#if (FLYBYWIRE_ENABLED == 1)

#include "FlyByWire.h"


uint8_t fbw_inject_pos = 0;
uint8_t fbw_inject[LENGTH_OF_PACKET];
int16_t fbw_pwm[NUM_INPUTS+1];

int16_t get_fbw_pwm(int16_t index)
{
	return fbw_pwm[index];
}
	
uint8_t get_fbw_pos(void)
{
	return fbw_inject_pos;
}	

void fbw_live_begin(void)
{
	fbw_inject_pos = 1 ; // The data parser never sees the first header value on UART
}


boolean fbw_live_received_byte(uint8_t inbyte)
{
	switch (fbw_inject_pos)
	{
	case 0:
		if (inbyte == 'F')
			fbw_inject_pos++;
		else
			return false;
		break;

	case 1:
		if (inbyte == 'b')
			fbw_inject_pos++;
		else
			return false;
		break;

	case 2:
		if (inbyte == 'W')
			fbw_inject_pos++;
		else
			return false;
		break;
			
	default:
		if (fbw_inject_pos < LENGTH_OF_PACKET)
		{
			fbw_inject[fbw_inject_pos++] = inbyte ;
		}
		else
		{
			return false;
		}
		break;
	} // switch
		
	return true;
}

void fbw_live_commit(void)
{
	fbw_live_commit_buf(fbw_inject);
}	

void fbw_live_commit_buf(uint8_t* buf)
{
	// [0,1,2] = "FbW" Header packet
	// [3,4] = AILERON_INPUT_CHANNEL (LSB, MSB)
	// [5,6] = ELEVATOR_INPUT_CHANNEL (LSB, MSB)
	// [7,8] = MODE_SWITCH_INPUT_CHANNEL (LSB, MSB)
	// [9,10] = RUDDER_INPUT_CHANNEL (LSB, MSB)
	// [11,12] = THROTTLE_INPUT_CHANNEL (LSB, MSB)
	
	uint8_t buf_index = LENGTH_OF_HEADER;
	uint16_t tempPWM;
	
	tempPWM = buf[buf_index++]; // LSB first
	tempPWM |= ((uint16_t)buf[buf_index++]) << 8;
	fbw_pwm[AILERON_INPUT_CHANNEL] = tempPWM;
	
	tempPWM = buf[buf_index++]; // LSB first
	tempPWM |= ((uint16_t)buf[buf_index++]) << 8;
	fbw_pwm[ELEVATOR_INPUT_CHANNEL] = tempPWM;
	
	tempPWM = buf[buf_index++]; // LSB first
	tempPWM |= ((uint16_t)buf[buf_index++]) << 8;
	fbw_pwm[MODE_SWITCH_INPUT_CHANNEL] = tempPWM;
	
	tempPWM = buf[buf_index++]; // LSB first
	tempPWM |= ((uint16_t)buf[buf_index++]) << 8;
	fbw_pwm[RUDDER_INPUT_CHANNEL] = tempPWM;
	
	tempPWM = buf[buf_index++]; // LSB first
	tempPWM |= ((uint16_t)buf[buf_index++]) << 8;
	fbw_pwm[THROTTLE_INPUT_CHANNEL] = tempPWM;
}	
	
#endif // (FLYBYWIRE_ENABLED)

#endif // _FLYBYWIRE_C_

