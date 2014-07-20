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

#include "CANDataManager.h"
#include "CANDataProtocol.h"
#include "libCAN.h"
#include "../MatrixPilot/defines.h"
#include "../CANInterface/CANInterface_defines.h"
#include "libCAN_IMU.h"
#include "../libDCM/libDCM_internal.h"

#include <string.h>		// for memcpy

#if (MAG_YAW_DRIFT == 1)
extern int magFieldEarth[3] ;
#endif
extern int udb_magOffset[3] ;
extern int offsetDelta[3] ;

// Transmission request flags
DATA_TX_REQUESTS	txRequests;

// Data buffer for IMU data
IMU_DATA IMUBuffer;

// Data buiffer for DCM internal data.
DCM_DATA DCMBuffer;

// Data buffer for flags
IMU_FLAGS flagsBuffer;

// Magnetometer data send is not the same for both directions
// Interface->IMU is measurement only using udb_magFieldBody
// IMU->Interface is all other results using magBuffer

// Data buffer for sending IMU data to Pilot
MAG_DATA  magBuffer;


// Copy of IMU navigation valid status
boolean	 IMU_nav_valid	= false;

void CAN_received_imu_data(void);
void CAN_received_dcm_data(void);
void CAN_received_flags(void);
void CAN_received_mag_data(void);

#define CAN_MAX_FAILSAFE_TIMEOUTS	10

//-----------------------------------------------------------------------------------------------------------------------
		// Failsafe
//-----------------------------------------------------------------------------------------------------------------------
int CAN_failsafe_count 	= 0;

void CAN_failsafe_clock(void)
{
	if(CAN_failsafe_count < CAN_MAX_FAILSAFE_TIMEOUTS)
	{
		CAN_failsafe_count++;
	}
	else
	{
		IMU_nav_valid = false;
		dcm_flags._.nav_capable = 0;
	}
}


//-----------------------------------------------------------------------------------------------------------------------
		// Broadcast data on CANbus
//-----------------------------------------------------------------------------------------------------------------------

// Copy the received radio inputs to the CAN buffer then request that they are sent.
void CAN_send_radio_inputs()
{
};


// request that servo outputs are sent
inline void CAN_send_servo_outputs()
{
	txRequests._.tx_req_servo_data = 1;
	CAN_request_send_next_data();
}


// request that servo outputs are sent
inline void CAN_send_flags()
{
	flagsBuffer.flags 		= flags.WW;
	flagsBuffer.dcm_flags 	= dcm_flags.B;
	flagsBuffer.udb_flags	= udb_flags.B;

	flagsBuffer.gps_nav_valid	= false;

// CAUTION TIMING OF FLAGS RESET AND THIS FUNCTION ARE IN CONTENTION
	flags._.save_origin = 0;

	txRequests._.tx_req_flags_data = 1;
	CAN_request_send_next_data();
}

inline void CAN_send_mag_data(void)
{
	txRequests._.tx_req_mag_data = 1;
	CAN_request_send_next_data();	
}

// Get the next data type block to transmit
// Fill out the variables in the protocol header for the block to transmit
// These include: Data type, data length, pointer to the data source buffer.
void CAN_get_next_tx_data_block(void)
{
	can_tx_bufferIndex = 0;

	if(txRequests._.tx_req_servo_data == 1)
	{
		ptxBuff = (unsigned int*) udb_pwOut;
		txRequests._.tx_req_servo_data = 0;
		can_tx_type = CONTROL_DATA_SERVO;
		can_tx_maxSize = MAX_OUTPUTS;
	}
	else if(txRequests._.tx_req_flags_data == 1)
	{
		ptxBuff = (unsigned int*) &flagsBuffer;
		txRequests._.tx_req_flags_data = 0;
		can_tx_type = CONTROL_DATA_FLAGS;
		can_tx_maxSize = (sizeof(flagsBuffer)/sizeof(int));
	}
#if ( MAG_YAW_DRIFT == 1 )
	else if(txRequests._.tx_req_mag_data == 1)
	{
		ptxBuff = (unsigned int*) udb_magFieldBody;
		txRequests._.tx_req_mag_data = 0;
		can_tx_type = CONTROL_DATA_MAG;
		can_tx_maxSize = 3;
	}
#endif
	else
	{
		can_tx_maxSize = 0;
		ptxBuff = NULL;
		can_tx_type = CONTROL_DATA_NONE;
	}
}



//-----------------------------------------------------------------------------------------------------------------------
		// Receive data from CANbus
//-----------------------------------------------------------------------------------------------------------------------

// Parses the raw data from the data protocol service
void CAN_received_data(unsigned char type, unsigned char index, unsigned char length, unsigned char* pData)
{
	unsigned int* pRxDataBuff;

	switch(	type )
	{
	case CONTROL_DATA_IMU:
		pRxDataBuff = (unsigned int*) &IMUBuffer;
		break;
	case CONTROL_DATA_FLAGS:
		pRxDataBuff = (unsigned int*) &flagsBuffer;
		break;
	case CONTROL_DATA_DCM:
		pRxDataBuff = (unsigned int*) &DCMBuffer;
		break;
	case CONTROL_DATA_MAG:
		pRxDataBuff = (unsigned int*) &magBuffer;
		break;
	}

	CAN_failsafe_count	= 0;

	memcpy( (unsigned char*) &pRxDataBuff[index], pData , length);
	length >>= 1;	// divide datalength to get number of int channels transmitted

	switch ( type )
	{
	case CONTROL_DATA_IMU:
		if( (index+length) >= (sizeof(IMU_DATA)/sizeof(int)) )
			CAN_received_imu_data();
		break;
	case CONTROL_DATA_DCM:
		if( (index+length) >= (sizeof(DCM_DATA)/sizeof(int)) )
			CAN_received_dcm_data();
		break;
	case CONTROL_DATA_FLAGS:
		CAN_received_flags();
		break;
	case CONTROL_DATA_MAG:
		if( (index+length) >= (sizeof(MAG_DATA)/sizeof(int)) )
			CAN_received_mag_data();
		break;
	}
}


//	unsigned int dead_reckon_enable		: 1 ;		// controlled by DCM startup
//	unsigned int reckon_req				: 1 ;		// controlled by DCM estYawDrift
//	unsigned int first_mag_reading		: 1 ;		// controlled by DCM startup
//	unsigned int mag_drift_req			: 1 ;		// controlled by DCM mag read complete callback
//	unsigned int yaw_req				: 1 ;		// controlled by DCM estYawDrift
//	unsigned int skip_yaw_drift			: 1 ;		// Controlled by UDB
//	unsigned int nav_capable			: 1 ;		// Status of IMU
//	unsigned int nmea_passthrough		: 1 ; /		// Controlled by UDB startup - Will not work

void CAN_received_flags(void)
{
	union dcm_fbts_byte { struct dcm_flag_bits _ ; char B ; } dcm_flags_temp ;

	dcm_flags_temp.B = flagsBuffer.dcm_flags;

	// Make sure that the dcm flags controlled by pilot are not overwritten
	dcm_flags_temp._.skip_yaw_drift 	= dcm_flags._.skip_yaw_drift;
	dcm_flags_temp._.nmea_passthrough 	= dcm_flags._.nmea_passthrough;
	dcm_flags.B = dcm_flags_temp.B;

	//	flags.WW 		= flagsBuffer.flags;
	//	udb_flags.B 	= flagsBuffer.udb_flags;
	IMU_nav_valid	= flagsBuffer.gps_nav_valid;
}


// Get the IMU flag for valid gps navigation
boolean IMU_gps_nav_valid(void)
{
	return IMU_nav_valid;
}



// Autopilot recieves, reformats and acts on servo data from CAN interface
void CAN_received_imu_data(void)
{
//	dcm_flags.B = IMUBuffer.dcm_flags;
	
	memcpy( rmat,&IMUBuffer.rmat,sizeof(IMUBuffer) );

	omegaAccum[0] = IMUBuffer.omegaAccum[0];
	omegaAccum[1] = IMUBuffer.omegaAccum[1];
	omegaAccum[2] = IMUBuffer.omegaAccum[2];

	omegagyro[0] = IMUBuffer.omegagyro[0];
	omegagyro[1] = IMUBuffer.omegagyro[1];
	omegagyro[2] = IMUBuffer.omegagyro[2];

	memcpy(&GPSlocation, &IMUBuffer.GPSlocation, sizeof(GPSlocation) );
	memcpy(&GPSvelocity, &IMUBuffer.GPSvelocity, sizeof(GPSvelocity) );

	memcpy(&velocity_thru_air, &IMUBuffer.velocity_thru_air, sizeof(velocity_thru_air) );

	estimatedWind[0] = IMUBuffer.estimatedWind[0];
	estimatedWind[1] = IMUBuffer.estimatedWind[1];
	estimatedWind[2] = IMUBuffer.estimatedWind[2];

	IMUlocationx.WW = IMUBuffer.IMUlocationx.WW;
	IMUlocationy.WW = IMUBuffer.IMUlocationy.WW;
	IMUlocationz.WW = IMUBuffer.IMUlocationz.WW;

	IMUvelocityx.WW = IMUBuffer.IMUvelocityx.WW;
	IMUvelocityy.WW = IMUBuffer.IMUvelocityy.WW;
	IMUvelocityz.WW = IMUBuffer.IMUvelocityz.WW;

	calculated_heading = IMUBuffer.calculated_heading;
	
	gps_data_age 			= IMUBuffer.gps_data_age;
	velocity_magnitude 		= IMUBuffer.velocity_magnitude;
	air_speed_magnitude		= IMUBuffer.air_speed_magnitude;

	lat_gps.WW 			= IMUBuffer.lat_gps.WW;
	long_gps.WW 		= IMUBuffer.long_gps.WW;
	alt_sl_gps.WW 		= IMUBuffer.alt_sl_gps.WW;

	lat_origin.WW 		= IMUBuffer.lat_origin.WW;
	long_origin.WW 		= IMUBuffer.long_origin.WW;
	alt_origin.WW 		= IMUBuffer.alt_origin.WW;

	IMUBuffer.filler	= 0xAA;
};


// Receives and parses dcm internal state data
void CAN_received_dcm_data(void)
{
	velocity_previous = DCMBuffer.velocity_previous;
	
	dirovergndHRmat[0] = DCMBuffer.dirovergndHRmat[0];
	dirovergndHRmat[1] = DCMBuffer.dirovergndHRmat[1];
	dirovergndHRmat[2] = DCMBuffer.dirovergndHRmat[2];
	
	dirovergndHGPS[0] = DCMBuffer.dirovergndHGPS[0];
	dirovergndHGPS[1] = DCMBuffer.dirovergndHGPS[1];
	dirovergndHGPS[2] = DCMBuffer.dirovergndHGPS[2];

	omega[0] = DCMBuffer.omega[0];
	omega[1] = DCMBuffer.omega[1];
	omega[2] = DCMBuffer.omega[2];


	locationErrorEarth[0] = DCMBuffer.locationErrorEarth[0];
	locationErrorEarth[1] = DCMBuffer.locationErrorEarth[1];
	locationErrorEarth[2] = DCMBuffer.locationErrorEarth[2];

	tow.WW  		= DCMBuffer.tow.WW;

	sog_gps.BB 		= DCMBuffer.sog_gps.BB;
	cog_gps.BB 		= DCMBuffer.cog_gps.BB;
	climb_gps.BB 	= DCMBuffer.climb_gps.BB;
	week_no.BB 		= DCMBuffer.week_no.BB;

	xpg.WW	 		= DCMBuffer.xpg.WW;
	ypg.WW			= DCMBuffer.ypg.WW; 
	zpg.WW 			= DCMBuffer.zpg.WW;

	xvg.BB			= DCMBuffer.xvg.BB;
	yvg.BB			= DCMBuffer.yvg.BB;
	zvg.BB			= DCMBuffer.zvg.BB;

	mode1			= DCMBuffer.mode1;
	mode2			= DCMBuffer.mode2;
	svs				= DCMBuffer.svs;
	hdop 			= DCMBuffer.hdop;
	
	forward_acceleration  = DCMBuffer.forward_acceleration;
	
	lat_cir 		= DCMBuffer.lat_cir;
	cos_lat 		= DCMBuffer.cos_lat;
};


void CAN_received_mag_data(void)
{
#if (MAG_YAW_DRIFT == 1)
	magFieldEarth[0] 	= magBuffer.magFieldEarth[0];
	magFieldEarth[1] 	= magBuffer.magFieldEarth[1];
	magFieldEarth[2] 	= magBuffer.magFieldEarth[2];

	udb_magOffset[0]	= magBuffer.udb_magOffset[0];
	udb_magOffset[1]	= magBuffer.udb_magOffset[1];
	udb_magOffset[2]	= magBuffer.udb_magOffset[2];

	offsetDelta[0]		= magBuffer.offsetDelta[0];
	offsetDelta[1]		= magBuffer.offsetDelta[1];
	offsetDelta[2]		= magBuffer.offsetDelta[2];
#endif
}

/****************************************************************************/
// This is part of the servo and radio interface software
//
// ServoInterface source code
//	http://code.google.com/p/rc-servo-interface
//
// Copyright 2010 ServoInterface Team
// See the AUTHORS.TXT file for a list of authors of ServoInterface.
//
// ServoInterface is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ServoInterface is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 
// along with ServoInterface.  If not, see <http://www.gnu.org/licenses/>.
//
// Many parts of ServoInterface use either modified or unmodified code
// from the MatrixPilot pilot project.
// The project also contains code for modifying MatrixPilot to operate
// with ServoInterface.
// For details, credits and licenses of MatrixPilot see the AUTHORS.TXT file.
// or see this website: http://code.google.com/p/gentlenav
/****************************************************************************/
