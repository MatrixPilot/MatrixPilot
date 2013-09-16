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
#include "config.h"
#include "../libDCM/gpsParseCommon.h"

#if (USE_TELELOG == 1)
#include "telemetry_log.h"
#endif

#if (USE_USB == 1)
#include "preflight.h"
#endif

#if (CONSOLE_UART != 0)
#include "console.h"
#endif

#if (USE_NETWORK != 0)
#define THIS_IS_STACK_APPLICATION
#include "../MyIpNetwork/MyIpNetwork.h"
#endif

int pic32_corcon;


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
	init_servoPrepare();
	init_states();
	init_behavior();
	init_serial();

#if (USE_NETWORK != 0)
	init_MyIpNetwork();
#endif

	if (setjmp())
	{
		// a processor exception occurred and we're resuming execution here 
		DPRINT("longjmp'd\r\n");
	}

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
#if (USE_NETWORK != 0)
		ServiceMyIpNetwork();
#endif
		udb_run();
	}

//	postflight();

	return 0;
}
