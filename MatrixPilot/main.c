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

#if (USE_TELELOG == 1)
#include "telemetry_log.h"
#endif

#if (USE_USB == 1)
#include "preflight.h"
#endif

#if (USE_CONFIGFILE == 1)
#include "config.h"
#endif

#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE)
#define THIS_IS_STACK_APPLICATION
#include "MyIpNetwork.h"
#endif

//	main program for testing the IMU.

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
	preflight();
#endif
	udb_init();
	dcm_init();
#if (USE_CONFIGFILE == 1)
	init_config();
#endif
	init_servoPrepare();
	init_states();
	init_behavior();
	init_serial();

#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE)
	init_MyIpNetwork();
#endif

	udb_run();

//	postflight();

	return 0;
}
