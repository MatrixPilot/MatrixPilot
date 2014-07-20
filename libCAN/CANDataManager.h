// SEE END OF FILE FOR LICENSE TERMS

/****************************************************************************
CANDataManager

Responsible for implementing CAN control data transfer library functions
 Transmit requests
 receive data callbacks
Control data is:
  Radio inputs
  Autopilot controls (yaw,pitch,roll etc...)
  Servo outputs

Control information can go in any direction between UDB and interface devices

Translates between CAN data and UDB data with relevant flag and format changes
  Using data type to call the correct callback
  Using data length to call callbacks at the right time
     Only call callbacks once the last packet is received
  Transfer transmit information into the CAN buffers and request transmission

CAUTION: With the existing implementation, care must be taken to only transfer
a set of control data in one direction.  
  ie. both ends should not transfer servo outputs. The result will be unpredicatable

****************************************************************************/

#include "libCAN_defines.h"

#ifndef CAN_DATA_MANAGER_H
#define CAN_DATA_MANAGER_H


// Trigger send servo outputs
void CAN_send_servo_outputs(void);

// Trigger sending of radio inputs
void CAN_send_radio_inputs(void);

// Trigger sending of radio inputs
void CAN_send_imu_data(void);

// Trigger sending of flag data
void CAN_send_flags(void);

// Trigger sending of dcm internal data
void CAN_send_dcm_data(void);

// Trigger sending of magnetometer measurement data
void CAN_send_mag_data(void);


// Get the IMU flag for valid gps navigation
boolean IMU_gps_nav_valid(void);

void CAN_failsafe_clock(void);

// Callback when radio inputs are received
extern void CAN_received_radio_inputs_callback(void);

// Callback when servo otuputs are received
extern void CAN_received_servo_outputs_callback(void);


// CAN_received_data - called by the protocol when data is received.
// type = the control type being received
// the index into the control type array
// length of data received in bytes
// pRXData = pointer to receive buffer.
extern void CAN_received_data(unsigned char type, unsigned char index, unsigned char length, unsigned char* pRXData);


#endif

