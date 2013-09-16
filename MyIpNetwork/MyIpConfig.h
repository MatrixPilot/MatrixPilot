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


#ifndef _MY_IP_CONFIG_H_
#define _MY_IP_CONFIG_H_

#include <stdint.h>
#include "../Microchip/NetConfig.h"
#include "../libDCM/libDCM.h"

// Types
//typedef uint8_t boolean;
//#define true    1
//#define false   0

// Select which Network modules you would like to Enable. Set to (0) to enable
#define NETWORK_USE_UART1               (0) // Forward UART1 data
#define NETWORK_USE_UART2               (0) // Forward UART2 data
#define NETWORK_USE_FLYBYDATALINK       (1) // Joystick -> flight surfaces (over the internet!)
#define NETWORK_USE_MAVLINK             (1) // Forward MAVLink data
#define NETWORK_USE_DEBUG               (1) // Debug - Simple Telnet in ASCII
#define NETWORK_USE_ADSB                (1)
#define NETWORK_USE_LOGO                (1)
#define NETWORK_USE_CAM_TRACKING        (1) // Camera Tracking, also set CAM_USE_EXTERNAL_TARGET_DATA=1
#define NETWORK_USE_GPSTEST             (1) // GPS spoof testing
#define NETWORK_USE_PWMREPORT           (1) // PWM pin states
#define NETWORK_USE_XPLANE              (1) // Talk directly to Xplane without a plug. Weeee!!!!!
#define NETWORK_USE_TELEMETRY_EXTRA     (1) // Same data as what SERIAL_UDB_EXTRA generates in telemetry.c
#define NETWORK_USE_GROUND_STATION      (1) // Reduced binary telemetry data for ground stations

#endif // _MY_IP_CONFIG_H_
