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

/*! \mainpage MatrixPilot Index Page
 *
 * \section intro_sec Introduction
 *
 * This is the introduction.
 *
 * \section install_sec Installation.
 *
 * \subsection step1 Step 1: Opening the box.
 *
 * etc..
 */

#include "defines.h"
#include "behaviour.h"
#include "telemetry.h"
#include "servoMix.h"
#include "servoPrepare.h"
#include "../libDCM/gpsParseCommon.h"
#include "../libUDB/serialIO.h"
#include "config.h"
#include "states.h"
#include "console.h"
#include "flightplan_waypoints.h"
#include "MAVLink.h"
#include "options_mavlink.h"
#include <setjmp.h>

//#include "../libFlashFS/filesys.h"

#if (USE_TELELOG == 1)
#include "telemetry_log.h"
#endif

#if (USE_USB == 1)
#include "preflight.h"
#endif

#if (CONSOLE_UART != 0)
#include "console.h"
#endif

#if (USE_MAVLINK == 1)
void parameter_table_init(void);
#endif

static jmp_buf buf;

int matrixpilot_init(void)
{
    dcm_flags._.udb_init_finished = 0;
#if (USE_USB == 1)
	preflight();    // perhaps this would be better called usb_init()
#endif
	gps_init();     // this sets function pointers so i'm calling it early for now
	udb_init();     // configure clocks and enables global interrupts
//	filesys_init(); // attempts to mount a file system
	config_init();  // reads .ini files otherwise initialises with defaults
	dcm_init();
#if (FLIGHT_PLAN_TYPE == FP_WAYPOINTS)
//	init_waypoints();
#endif
	servoMix_init();
	servoPrepare_init();
	init_states();
	init_behavior();
	telemetry_init();
	mavlink_init();
    dcm_flags._.udb_init_finished = 1;

#ifdef _MSC_VER
#if (USE_MAVLINK == 1)
	parameter_table_init();
#endif // (USE_MAVLINK == 1)
#endif // _MSC_VER
	return 0;
}

int matrixpilot_loop(void)
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
	return 0;
}

#if (SILSIM == 1)
int mp_argc;
char **mp_argv;
int main(int argc, char** argv)
{
	// keep these values available for later
	mp_argc = argc;
	mp_argv = argv;
#else
int main(void)
{
	mcu_init();     // initialise the processor specific registers
#endif
	matrixpilot_init();

	if (setjmp(buf))
	{
		// a processor exception occurred and we're resuming execution here
		DPRINT("longjmp'd\r\n");
	}
	while (1)
	{
		matrixpilot_loop();
	}
	return 0;
}
