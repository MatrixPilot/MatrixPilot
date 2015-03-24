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

//#ifdef USE_FREERTOS
#if defined USE_FREERTOS || defined PX4

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

xSemaphoreHandle xSemaphoreGPS;

void TriggerGPS(void)
{
	xSemaphoreGiveFromISR(xSemaphoreGPS, NULL);
}

// Got a character from the GPS
//void udb_gps_callback_received_byte(uint8_t rxchar)
//{
//}

static void TaskGPS(void* pvParameters)
{
	(void)pvParameters;
	DPRINT("TaskGPS\r\n");
	while (1)
	{
		xSemaphoreTake(xSemaphoreGPS, portMAX_DELAY);
		udb_led_toggle(LED_RED);
void udb_background_callback_triggered(void);
//		udb_background_callback_triggered();
	}
}

void TaskGPS_Init(void)
{
	xTaskHandle xHandle = NULL;

	vSemaphoreCreateBinary(xSemaphoreGPS);
	configASSERT(xSemaphoreGPS);
	xTaskCreate(TaskGPS, (signed portCHAR*)"GPS", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, &xHandle);
	configASSERT(xHandle);
}

#endif // USE_FREERTOS
