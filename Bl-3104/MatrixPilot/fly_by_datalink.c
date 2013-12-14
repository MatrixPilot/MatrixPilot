#ifndef _FLYBYDATALINK_C_
#define _FLYBYDATALINK_C_

#include "defines.h"
#if (FLY_BY_DATALINK_ENABLED == 1)

#include "fly_by_datalink.h"


uint8_t fbdl_inject_pos = 0;
uint8_t fbdl_inject[LENGTH_OF_PACKET];
int16_t fbdl_pwm[NUM_INPUTS + 1];

int16_t get_fbdl_pwm(int16_t index)
{
  return fbdl_pwm[index];
}

uint8_t get_fbdl_pos(void)
{
  return fbdl_inject_pos;
}

void fbdl_live_begin(void)
{
  fbdl_inject_pos = 1; // The data parser never sees the first header value on UART
}

boolean fbdl_live_received_byte(uint8_t inbyte)
{
  switch (fbdl_inject_pos)
  {
  case 0:
    if (inbyte == 'F')
      fbdl_inject_pos++;
    else
      return false;
    break;

  case 1:
    if (inbyte == 'b')
      fbdl_inject_pos++;
    else
      return false;
    break;

  case 2:
    if (inbyte == 'W')
      fbdl_inject_pos++;
    else
      return false;
    break;

  default:
    if (fbdl_inject_pos < LENGTH_OF_PACKET)
    {
      fbdl_inject[fbdl_inject_pos++] = inbyte;
    }
    else
    {
      return false;
    }
    break;
  } // switch

  return true;
}

void fbdl_live_commit(void)
{
  fbdl_live_commit_buf(fbdl_inject);
}

void fbdl_live_commit_buf(uint8_t* buf)
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
  tempPWM |= ((uint16_t) buf[buf_index++]) << 8;
  fbdl_pwm[AILERON_INPUT_CHANNEL] = tempPWM;

  tempPWM = buf[buf_index++]; // LSB first
  tempPWM |= ((uint16_t) buf[buf_index++]) << 8;
  fbdl_pwm[ELEVATOR_INPUT_CHANNEL] = tempPWM;

  tempPWM = buf[buf_index++]; // LSB first
  tempPWM |= ((uint16_t) buf[buf_index++]) << 8;
  fbdl_pwm[MODE_SWITCH_INPUT_CHANNEL] = tempPWM;

  tempPWM = buf[buf_index++]; // LSB first
  tempPWM |= ((uint16_t) buf[buf_index++]) << 8;
  fbdl_pwm[RUDDER_INPUT_CHANNEL] = tempPWM;

  tempPWM = buf[buf_index++]; // LSB first
  tempPWM |= ((uint16_t) buf[buf_index++]) << 8;
  fbdl_pwm[THROTTLE_INPUT_CHANNEL] = tempPWM;
}

#endif // (FLY_BY_DATALINK_ENABLED)

#endif // _FLYBYDATALINK_C_

