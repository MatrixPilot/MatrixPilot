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

#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE)
    #define THIS_IS_STACK_APPLICATION
    #include "TCPIP_Stack/TCPIP.h"
    #include "MyIpNetwork.h"
#endif

void mcu_init(void);
void preflight(void);
void console(void);


//	main program for testing the IMU.

#if (SILSIM == 1)
int mp_argc;
char **mp_argv;
int main(int argc, char** argv)
{
	// keep thees values available for later
	mp_argc = argc;
	mp_argv = argv;
#else
int main (void)
{
#endif
	mcu_init();
	log_init();
	udb_init() ;
	dcm_init() ;
	init_servoPrepare() ;
	init_states() ;
	init_behavior() ;
	init_serial() ;

    #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE)
	// should be done after init_serial() for error messages
    init_MyIpNetwork() ;
    #endif

	preflight();

	while (1) {
		console();
//		write_logbuf();
		udb_run() ;
	}
	return 0 ;
}

#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE)
void IpNetworkActivity(void)
{
	LED_ORANGE ^= 1;
}

void IpNetworkConnected(int IsConnected)
{
	if (IsConnected)
		LED_TCP_CONNECTED = LED_ON;
	else
		LED_TCP_CONNECTED = LED_OFF;
}
#endif
