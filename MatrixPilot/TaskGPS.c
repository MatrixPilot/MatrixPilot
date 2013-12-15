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
#include "../libDCM/gpsParseCommon.h"
#include "../libUDB/heartbeat.h"
#include "config.h"

#ifdef USE_FREERTOS

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

//static xSemaphoreHandle xSemaphoreGPS = 0;
static xQueueHandle hRxQ = 0;
static xQueueHandle hTxQ = 0;
const int uxQueueLength = 50;

//void TriggerGPS(void)
//{
//	xSemaphoreGiveFromISR(xSemaphoreGPS, NULL);
//}

#include "../libUDB/sio.h"

SIO_DEFINE(gps, 1)

//void udb_init_GPS(void)
//{
//	gps_sio_init(udb_gps_callback_received_byte, INT_PRI_U1RX, udb_gps_callback_get_byte_to_send, INT_PRI_U1RX);
//}

void udb_gps_set_rate(int32_t rate)
{
	gps_sio_set_baud(rate);
}

void udb_gps_start_sending_data(void)
{
	gps_sio_start_tx();
}


// Got a character from the GPS
void udb_gps_callback_received_byte(uint8_t rxchar)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	if (hRxQ != 0)
	{
		xQueueSendFromISR(hRxQ, &rxchar, &xHigherPriorityTaskWoken);
	}
	if (xHigherPriorityTaskWoken != pdFALSE)
	{
		taskYIELD();
	}
}

static void TaskGPS(void* pvParameters)
{
	DPRINT("TaskGPS\r\n");
	gps_sio_init(udb_gps_callback_received_byte, INT_PRI_U1RX, udb_gps_callback_get_byte_to_send, INT_PRI_U1RX);
	while (1)
	{
		uint8_t rxchar;
		if (xQueueReceive(hRxQ, &rxchar, portMAX_DELAY))
		{
void udb_gps_msg_parse(uint8_t rxchar);
			udb_gps_msg_parse(rxchar);
		}
		else
		{
		}
	}
}

void TaskGPS_Init(void)
{
	xTaskHandle xHandle = NULL;

	hRxQ = xQueueCreate(uxQueueLength, (unsigned portBASE_TYPE)sizeof(signed char));
	configASSERT(hRxQ);
	if (hRxQ == 0)
    {
		DPRINT("Failed to create GPS Rx queue\r\n");
    }
	hTxQ = xQueueCreate(uxQueueLength, (unsigned portBASE_TYPE)sizeof(signed char));
	configASSERT(hTxQ);
	if (hTxQ == 0)
    {
		DPRINT("Failed to create GPS Tx queue\r\n");
    }
//	vSemaphoreCreateBinary(xSemaphoreGPS);
	xTaskCreate(TaskGPS, (signed portCHAR*)"GPS", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, &xHandle);
	configASSERT(xHandle);
}

#endif // USE_FREERTOS
