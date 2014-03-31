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
#include "../libUDB/heartbeat.h"

#ifdef USE_FREERTOS

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "TaskLRS.h"

void rx_poll(void);
boolean link_poll(void);
void sample_rssi(void);
uint16_t rfmGetIntFlags(void);

static xSemaphoreHandle xSemaphoreLRS = NULL;
int foo = 0;


uint8_t rfmGetInt1(void);
uint8_t rfmGetInt2(void);


#define RF_AVAILABLE    0
#define RF_TRANSMIT_REQ 1
#define RF_TRANSMIT_SIG 2
#define RF_RECEIVE_REQ  3
#define RF_RECEIVE_SIG  4

extern volatile uint8_t RF_Mode;

void TaskLRS_Trigger(void);
void rx_reset_bind(void);


uint8_t bindReceive(uint32_t timeout)
{
	portTickType xDelay = 100 / portTICK_RATE_MS;   // Block for 100ms
	int8_t result = 0;

	uint16_t _timeout = timeout / xDelay; // convert the 500ms in 25ms blocks
//	_timeout = _timeout * 2; // convert the 500ms in 25ms blocks

	DPRINT("bindReceive() xDelay %u\r\n", xDelay);
	DPRINT("bindReceive() portTICK_RATE_MS %u\r\n", portTICK_RATE_MS);
	DPRINT("bindReceive() timeout %lu\r\n", timeout);
	DPRINT("bindReceive() _timeout %u\r\n", _timeout);

	init_rfm(1); // initialise in bind mode
//	to_rx_mode(); // __bindReceive()
	to_rx_bind_mode(); // __bindReceive()
	DPRINT("Waiting bind\n");

	while (1)
	{
		if (xSemaphoreTake(xSemaphoreLRS, xDelay) == pdTRUE)
		{
			uint16_t rfmIntFlags = rfmGetIntFlags(); // returns int1 in highbyte
			if (rfmIntFlags & 0x0200) // Valid Packet Received
			{
				result = bindReceivePkt();
				if (result < 0) {
					_timeout = 0; // do not timeout
	DPRINT("bindReceive() do not timeout...\r\n");
				}
				if (result > 0) {
	DPRINT("bindReceive() BIND COMPLETED\r\n");
					return 1; // bind completed
				}
			}
			if (rfmIntFlags & 0x0400) // Packet Sent
			{
				rx_reset_bind(); // __bindReceive()
//	DPRINT("Sent\r\n");
			}
 
			if (rfmIntFlags & 0x0080) // Word Sync Detected
			{
//	DPRINT("Sync\r\n");
			}

//	_timeout = timeout / xDelay; // convert the 500ms in 25ms blocks
//	_timeout = _timeout * 2; // convert the 500ms in 25ms blocks

		}
		if (_timeout)
		{
	DPRINT("_timeout %u\r\n", _timeout);
			if (!(--_timeout))
			{
	DPRINT("bindReceive() BIND CONNECT TIMEOUT\r\n");
				return 0; // stop trying to bind
			}
		}
	}
	DPRINT("bindReceive() EXIT\r\n");
	return 0;
}


static void TaskLRS(void* pvParameters)
{
	portTickType xDelay;
	boolean linkAcquired = 0;
	uint8_t first_pkt_wait_cnt = 0;

	uint8_t hopcount = 6;

	(void)pvParameters;

	DPRINT("TaskLRS\r\n");

	if (!lrs_setup())
	{
		DPRINT("lrs_setup() failed\r\n");
		for (;;)
		{
			portYIELD();
		}
	}

	xDelay = 26 / portTICK_RATE_MS;   // Block for 26ms. (getInterval(&bind_data) + 1000)
//	xDelay = 31 / portTICK_RATE_MS;   // Block for 31ms. (time since end of transmission to expected next packet rx completion)

	DPRINT("TaskLRS, setup complete\r\n");
	while (1)
	{
//OLS3 = !OLS3;
		if (xSemaphoreTake(xSemaphoreLRS, xDelay) == pdTRUE)
		{
OLS3 = 0;
#if 1
			uint16_t rfmIntFlags = rfmGetIntFlags(); // returns int1 in highbyte
#else
			uint8_t rfmInt1 = rfmGetInt1();
			uint8_t rfmInt2 = rfmGetInt2();
			uint16_t rfmIntFlags = ((uint16_t)rfmInt1 << 8) + rfmInt2;
#endif
//			if (foo++ > 50)
//			{
//				foo = 0;
//				DPRINT("I %04x\r\n", rfmIntFlags);
//				DPRINT("I1 %02x - I2 %02x\r\n", rfmInt1, rfmInt2);
//			}
			if (rfmIntFlags & 0x0400) // Packet Sent
			{
  if (RF_Mode == RF_TRANSMIT_REQ) {
    RF_Mode = RF_TRANSMIT_SIG;
  }
//      RF_Mode = RF_RECEIVE_REQ; // _bindReceive()
//      rx_reset(); // _bindReceive()
			}
			if (rfmIntFlags & 0x0200) // Valid Packet Received
			{
  if (RF_Mode == RF_RECEIVE_REQ) {
    RF_Mode = RF_RECEIVE_SIG;
  }
OLS1 = 1;
				rx_poll();
			    channel_hop();
				linkAcquired = true;
OLS1 = 0;
			}
			if (rfmIntFlags & 0x0080) // Word Sync Detected
			{
//OLS2 = !OLS2;
OLS2 = 1;
				sample_rssi();
OLS2 = 0;
			}
			if (rfmIntFlags & 0x0040) // Valid Preamble Detected
			{
//OLS2 = !OLS2;
			}
			if (rfmIntFlags & 0x0008) // Wake-Up-Timer
			{
			}
			if (rfmIntFlags & 0x0004) // Low Battery Detect
			{
			}
		}
		else // semaphore timeout
		{
OLS2 = 1;
delay_ms(1); // just so i can see the OLS pulse..
			if (linkAcquired)
			{
				if (link_poll())
				{
				    channel_hop();
				}
			}
			else
			{
    // Waiting for first packet, hop slowly
    			if (first_pkt_wait_cnt++ > hopcount) {
					first_pkt_wait_cnt = 0;
				    channel_hop();
    			}
			}
OLS2 = 0;
		}
	}
}

void TaskLRS_Trigger(void)
{
	static portBASE_TYPE xHigherPriorityTaskWoken;

	configASSERT(xSemaphoreLRS);
OLS3 = 1;
	xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(xSemaphoreLRS, &xHigherPriorityTaskWoken);
	if (xHigherPriorityTaskWoken == pdTRUE)
	{
		portYIELD();
	}
}

void TaskLRS_Init(void)
{
	xTaskHandle xHandle = NULL;
	configASSERT(portTICK_RATE_MS);
	vSemaphoreCreateBinary(xSemaphoreLRS);
	configASSERT(xSemaphoreLRS);
	xTaskCreate(TaskLRS, (signed portCHAR*)"LRS", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, &xHandle);
	configASSERT(xHandle);
}

#endif // USE_FREERTOS
