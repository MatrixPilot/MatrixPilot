// This file is part of the MatrixPilotQuad firmware.
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

#include "../libDCM/libDCM.h"
#include "options.h"
#include "debug.h"


int freq_mc = 0;
int freq_adc = 0;
int freq_rmat = 0;
int freq_2hz = 0;
int freq_task = 0;

int freq = 0;
int count = 0;
int one_hertz = 0;
int two_hertz = 0;
int one_hertz_2 = 0;
int two_hertz_2 = 0;

/*
double f2f(unsigned int a)
{
	double b;

    b = (double) (((long)a)/((long)((2^14)-1)));

	return b;
}

extern float Fract2Float (fractional);

void print_pid_gains(void)
{
	float r = 0.5;

//	printf("\r\n");
//	printf("0.5 = %f\n", Fract2Float(Float2Fract(r))); 	
//	printf("0.5 = %i\n", Float2Fract(r)); 	
//	printf("0.5 = %f\n", Fract2Float(RMAX)); 	
//	printf("\r\n");

	printf("TILT_KP_INDEX %f\r\n", Fract2Float(pid_gains[TILT_KP_INDEX]));
	printf("RATE_KP_INDEX %f\r\n", Fract2Float(pid_gains[RATE_KP_INDEX]));
	printf("RATE_KD_INDEX %f\r\n", Fract2Float(pid_gains[RATE_KD_INDEX]));
	printf(" YAW_KP_INDEX %f\r\n", Fract2Float(pid_gains[YAW_KP_INDEX]));
	printf(" YAW_KD_INDEX %f\r\n", Fract2Float(pid_gains[YAW_KD_INDEX]));
	printf("ACCEL_K_INDEX %f\r\n", Fract2Float(pid_gains[ACCEL_K_INDEX]));
	printf("ACRO_KP_INDEX %f\r\n", Fract2Float(pid_gains[ACRO_KP_INDEX]));
	printf("TILT_KI_INDEX %f\r\n", Fract2Float(pid_gains[TILT_KI_INDEX]));
	printf(" YAW_KI_INDEX %f\r\n", Fract2Float(pid_gains[YAW_KI_INDEX]));
	printf("RATE_KI_INDEX %f\r\n", Fract2Float(pid_gains[RATE_KI_INDEX]));

#define RMAX15 0b0110000000000000	//	1.5 in 2.14 format

//	printf("RMAX %i, TILT_KP %f\r\n", RMAX, TILT_KP);
	printf("RMAX %f, RMAX15 %f\r\n", Fract2Float(RMAX), Fract2Float(RMAX15));
	printf("\r\n");
}
 */

boolean sendGPS;
boolean sendGains;unsigned int lowVoltageWarning;union longww primary_voltage;
void send_telemetry(void)
{
}

#ifdef MP_QUAD

void flightplan_live_begin(void)
{
}

void flightplan_live_received_byte(void)
{
}
void flightplan_live_commit(void)
{
}
#endif // MP_QUAD

/*
void use_fixed_origin(void)
{
}

void get_fixed_origin(void)
{
}

void run_flightplan(void)
{
}

void init_flightplan(void)
{
}
 */
//void testproc_init(void)
//{
//}
