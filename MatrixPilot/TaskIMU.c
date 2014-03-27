// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2011 MatrixPilot Team
// See the AUTHORS.TXT file for a list of authors of MatrixPilot.
//
// MatrixPilot is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MatrixPilot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MatrixPilot.  If not, see <http://www.gnu.org/licenses/>.


#include "defines.h"
//#include "../libDCM/libDCM.h"
#include "../libUDB/libUDB_internal.h"
#include "../libUDB/heartbeat.h"

#ifdef USE_FREERTOS

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "TaskIMU.h"

static xSemaphoreHandle xSemaphoreIMU = NULL;

uint16_t heartbeat_counter = 0;

inline void pulse(void)
{
//	heartbeat_counter = (heartbeat_counter+1) % HEARTBEAT_MAX;

/***** MOVED THIS FROM THE HEARTBEAT ISR ****/
	// Call the periodic callback at 40 Hz
	if (udb_heartbeat_counter % (HEARTBEAT_HZ/40) == 0)
	{
		// THIS IS ACTUALLY THE FLIGHT STATE MACHINE IN MODULE STATES.C
		udb_heartbeat_40hz_callback(); // this was called udb_background_callback_periodic()
	}

/***** WE STARTED OFF AT THE libUDB LAYER ****/

//	LED_BLUE = LED_OFF;     // indicates logfile activity

#if (NORADIO != 1)
	// 20Hz testing of radio link
	if ((udb_heartbeat_counter % (HEARTBEAT_HZ/20)) == 1)
	{
		radioIn_failsafe_check();
	}
	// Computation of noise rate
	// Noise pulses are counted when they are detected, and reset once a second
	if (udb_heartbeat_counter % (HEARTBEAT_HZ/1) == 1)
	{
		radioIn_failsafe_reset();
	}
#endif // NORADIO

	calculate_analog_sensor_values();
	udb_callback_read_sensors();
//void udb_callback_read_sensors(void)
//{
//	read_gyros(); // record the average values for both DCM and for offset measurements
//	read_accel();
//}
	udb_flags._.a2d_read = 1; // signal the A/D to start the next summation

/***** THEN CALLED UP TO THE libDCM LAYER ****/
//	udb_heartbeat_callback(); // this was called udb_servo_callback_prepare_outputs()
//void udb_heartbeat_callback(void)

	if (udb_heartbeat_counter % (HEARTBEAT_HZ / 40) == 0)
	{
		do_I2C_stuff(); // TODO: this should always be be called at 40Hz
	}

/***** THEN CALLED UP TO THE MatrixPilot LAYER ****/
//	dcm_heartbeat_callback();    // this was called dcm_servo_callback_prepare_outputs();
//void dcm_heartbeat_callback(void)

//  when we move the IMU step to the MPU call back, to run at 200 Hz, remove this
	if (dcm_flags._.calib_finished)
	{
		dcm_run_imu_step();
	}

	if (dcm_flags._.calib_finished)
	{
		// MOVED HERE FROM flight_controller()
		if (udb_heartbeat_counter % (HEARTBEAT_HZ/40) == 0)
		{
			flight_mode_switch_2pos_poll(); // we always want this called at 40Hz
		}

		flight_controller();
#if (SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK)
		if (udb_heartbeat_counter % (HEARTBEAT_HZ/40) == 0)
		{
			mavlink_output_40hz();
		}
#else
		// This is a simple check to send telemetry at 8hz
		if (udb_heartbeat_counter % (HEARTBEAT_HZ/8) == 0)
		{
// RobD			flight_state_8hz();
			serial_output_8hz();
		}
#endif
	}
	else
	{
		// otherwise, there is not anything to do
		manualPassthrough();                // Allow manual control while starting up
	}
	
	osd_run_step();

/***** FINISHED OFF BACK IN THE libDCM LAYER ****/
	if (!dcm_flags._.init_finished)
	{
		if (udb_heartbeat_counter % (HEARTBEAT_HZ / 40) == 0)
		{
			dcm_run_init_step(udb_heartbeat_counter / (HEARTBEAT_HZ / 40));
			gps_run_init_step(udb_heartbeat_counter / (HEARTBEAT_HZ / 40));
		}
	}

#if (HILSIM == 1)
	send_HILSIM_outputs();
#endif

/***** FINALLY FINISHING OFF WHERE WE STARTED IN THE libUDB LAYER ****/
	if (udb_heartbeat_counter % (HEARTBEAT_HZ/40) == 0)
	{
#if (USE_I2C1_DRIVER == 1)
		I2C1_Trigger();
#endif
#if (USE_NV_MEMORY == 1)
		nv_memory_service_trigger();
		storage_service_trigger();
		data_services_trigger();
#endif
#if (USE_FLEXIFUNCTION_MIXING == 1)
		flexiFunctionServiceTrigger();
#endif
	}
}


static void TaskIMU(void* pvParameters)
{
	(void)pvParameters;
	DPRINT("TaskIMU\r\n");
	while (1)
	{
		xSemaphoreTake(xSemaphoreIMU, portMAX_DELAY);
		pulse();
//		IMU_update();  // this one at full speed
//		FSM_update();  // this one at only 40Hz
	}
}

void TriggerIMU(void)
{
	xSemaphoreGiveFromISR(xSemaphoreIMU, NULL);
}

void TaskIMU_Init(void)
{
	xTaskHandle xHandle = NULL;

	vSemaphoreCreateBinary(xSemaphoreIMU);
	configASSERT(xSemaphoreIMU);
	xTaskCreate(TaskIMU, (signed portCHAR*)"IMU", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+3, &xHandle);
	configASSERT(xHandle);
}

#endif // USE_FREERTOS
