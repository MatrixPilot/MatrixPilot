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
#include "behaviour.h"
#include "servoPrepare.h"
#include "../libDCM/gpsParseCommon.h"
#include "config.h"
#include "flightplan-waypoints.h"
#include <setjmp.h>

#if (USE_TELELOG == 1)
#include "telemetry_log.h"
#endif

#if (USE_USB == 1)
#include "preflight.h"
#endif

#if (CONSOLE_UART != 0)
#include "console.h"
#endif

#if (SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK) 
void parameter_table_init(void);
#endif

static jmp_buf buf;

#if (SILSIM == 1)
int mp_argc;
char **mp_argv;
int16_t angleOfAttack;
int16_t tiltError[3];
int16_t rotationRateError[3];
int16_t desiredRotationRateRadians[3];
int main(int argc, char** argv)
{
	// keep these values available for later
	mp_argc = argc;
	mp_argv = argv;
#else
int main(void)
{
	mcu_init();
#endif
#if (USE_TELELOG == 1)
	log_init();
#endif
#if (USE_USB == 1)
	preflight();    // perhaps this would be better called usb_init()
#endif
	gps_init();     // this sets function pointers so i'm calling it early for now
	udb_init();
	dcm_init();
	init_config();  // this will need to be moved up in order to support runtime hardware options
#if (FLIGHT_PLAN_TYPE == FP_WAYPOINTS)
	init_waypoints();
#endif
	init_servoPrepare();
	init_states();
	init_behavior();
	init_serial();

	if (setjmp(buf))
	{
		// a processor exception occurred and we're resuming execution here 
		DPRINT("longjmp'd\r\n");
	}

#ifdef _MSC_VER
#if (SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK) 
	parameter_table_init();
#endif // SERIAL_OUTPUT_FORMAT
#endif // _MSC_VER

	while (1)
	{
#if (USE_TELELOG == 1)
		telemetry_log();
#endif
#if (USE_USB == 1)
		USBPollingService();
#endif
#if (CONSOLE_UART != 0 && SILSIM == 0)
		console();
#endif
		udb_run();
	}
	return 0;
}
