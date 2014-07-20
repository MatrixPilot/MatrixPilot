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

#if (USE_TELELOG == 1)
#include "telemetry_log.h"
#endif

#if (USE_USB == 1)
#include "preflight.h"
#endif

#if (CONSOLE_UART != 0)
#include "console.h"
#endif

void TaskIMU_Init(void);
void TaskGPS_Init(void);

#ifdef USE_FREERTOS

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "../libUDB/barometer.h"

static void TaskI2C(void* pvParameters)
{
	DPRINT("TaskI2C\r\n");
	while (1)
	{
		vTaskDelay(1000 / portTICK_RATE_MS);
#if (BAROMETER_ALTITUDE == 1)
		rxBarometer(NULL);
#endif // BAROMETER_ALTITUDE
//		udb_led_toggle(LED_RED);
	}
}

static void TaskInit(void* pvParameters)
{
	DPRINT("TaskInit\r\n");

	TaskIMU_Init();
	TaskGPS_Init();
//	xTaskCreate(TaskI2C, (signed portCHAR*)"I2C", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);
	vTaskDelete(NULL);
}

void init_tasks(void)
{
	xTaskCreate(TaskInit, (signed portCHAR*)"INI", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+3, NULL);
}

#endif // USE_FREERTOS
