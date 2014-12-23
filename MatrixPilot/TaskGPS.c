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
#include "ring_buf.h"

#ifdef USE_FREERTOS

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

static xSemaphoreHandle xSemaphoreGPS = 0;
//static xQueueHandle hRxQ = 0;
//static xQueueHandle hTxQ = 0;
//const int uxQueueLength = 50;
//const int uxQueueLength = 10;

#include "../libUDB/sio.h"

SIO_DEFINE(gps, 1)

ring_buf_t ring_buf;

//void udb_init_GPS(void)
//{
//	gps_sio_init(udb_gps_callback_received_byte, INT_PRI_U1RX, udb_gps_callback_get_byte_to_send, INT_PRI_U1TX);
//}

#if (SILSIM == 0)
void udb_gps_set_rate(int32_t rate)
{
//	DPRINT("udb_gps_set_rate %li\r\n", rate);
	gps_sio_set_baud(rate);
}

void udb_gps_start_sending_data(void)
{
	gps_sio_start_tx();
}
#endif // SILSIM


// Got a character from the GPS
void udb_gps_callback_received_byte(uint8_t rxchar)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
/*
	if (hRxQ != 0)
	{
		xQueueSendFromISR(hRxQ, &rxchar, &xHigherPriorityTaskWoken);
		if (xHigherPriorityTaskWoken != pdFALSE)
		{
			taskYIELD();
		}
	}
 */
	if (xSemaphoreGPS != NULL)
	{
		ring_buf_put(&ring_buf, (unsigned char)rxchar);
		xSemaphoreGiveFromISR(xSemaphoreGPS, &xHigherPriorityTaskWoken);
		if (xHigherPriorityTaskWoken != pdFALSE)
		{
			taskYIELD();
		}
	}
}

static void TaskGPS(void* pvParameters)
{
	(void)pvParameters;
	DPRINT("TaskGPS\r\n");

	gps_sio_set_baud(38400);
	gps_sio_init(udb_gps_callback_received_byte, INT_PRI_U1RX, udb_gps_callback_get_byte_to_send, INT_PRI_U1TX);

	while (1)
	{
		int result;
		uint8_t rxchar;

//		if (xQueueReceive(hRxQ, &rxchar, portMAX_DELAY))
//		if (xQueueReceive(hRxQ, &rxchar, 1000))
//		if (xSemaphoreTake(xSemaphoreGPS, portMAX_DELAY) == pdTRUE)
		if (xSemaphoreTake(xSemaphoreGPS, 10000) == pdTRUE)
		{
void udb_gps_msg_parse(uint8_t rxchar);
			while ((result = ring_buf_get(&ring_buf)) != -1)
			{
				rxchar = result;
//				DPRINT("%c", rxchar);
				udb_gps_msg_parse(rxchar);
			}
		}
		else
		{
			DPRINT("gps_rx timeout\r");
		}
	}
}

void TaskGPS_Init(void)
{
	xTaskHandle xHandle = NULL;

	ring_buf_init(&ring_buf);
/*
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
 */
	vSemaphoreCreateBinary(xSemaphoreGPS);
	configASSERT(xSemaphoreGPS);
	xTaskCreate(TaskGPS, (signed portCHAR*)"GPS", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, &xHandle);
	configASSERT(xHandle);
}

#endif // USE_FREERTOS
