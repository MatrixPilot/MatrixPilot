#include "libCAN_defines.h"

#ifndef CAN_DATA_PROTOCOL_H
#define CAN_DATA_PROTOCOL_H

// Called to parse received CAN data
void CAN_parse_received_data(void);

// Called to start the next data send,  including constructing the CAN data packet.
void CAN_request_send_next_data(void);

extern void CAN_get_next_tx_data_block(void);

// A pointer to the data to send
extern unsigned int* 			ptxBuff;

// An index into the next part of the data block to be sent
// index is in sizeof(uint)
extern unsigned char  			can_tx_bufferIndex;

// Maximum size of the transmitting data block
extern unsigned char  			can_tx_maxSize;

// The type of the data being sent
extern unsigned char  			can_tx_type;


typedef struct tagRADIO_DATA_BITS
{
	unsigned int 	PWM				: 15;
	unsigned int 	channelFound	: 1;
} RADIO_DATA_BITS;


typedef union 
{
	RADIO_DATA_BITS	_;
	unsigned int	W;
} RADIO_DATA;


typedef struct tagIMU_FLAGS
{
	int 	flags ;
	char 	dcm_flags;
	char	udb_flags;
	boolean	gps_nav_valid;
	char	filler;
} IMU_FLAGS;


typedef struct tagDATA_TX_REQUEST_BITS
{
	unsigned int	tx_req_radio_data		: 1;
	unsigned int	tx_req_servo_data		: 1;
	unsigned int	tx_req_imu_data			: 1;
	unsigned int	tx_req_flags_data		: 1;
	unsigned int	tx_req_dcm_data			: 1;
	unsigned int	tx_req_mag_data			: 1;
	unsigned int							: 2;
} DATA_TX_REQUEST_BITS;


typedef union
{
	unsigned char			reqs;
	DATA_TX_REQUEST_BITS	_;
} DATA_TX_REQUESTS;


typedef enum
{
	CONTROL_DATA_NONE,
	CONTROL_DATA_SERVO,
	CONTROL_DATA_RADIO,
	CONTROL_DATA_AP_CONTROL,
	CONTROL_DATA_FLAGS,
	CONTROL_DATA_IMU,
	CONTROL_DATA_DCM,
	CONTROL_DATA_MAG
} CONTROL_DATA_TYPES;

#endif
