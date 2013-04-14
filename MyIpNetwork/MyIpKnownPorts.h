
#ifndef MYIPKNOWNPORTS_H
#define	MYIPKNOWNPORTS_H




//Suggested standard port numbers for MyIp services.
//These can be changed but best to just use these,
//unless there's a valid reason to change them.
//For each service, default to _PORT_1 and use _2 and _3
//if additional ports are needed for that service.
#define MYIP_UART1_PORT_1           3000
#define MYIP_UART1_PORT_2           3001
#define MYIP_UART1_PORT_3           3002
#define MYIP_UART2_PORT_1           3003
#define MYIP_UART2_PORT_2           3004
#define MYIP_UART2_PORT_3           3005
#define MYIP_FLYBYWIRE_PORT_1       3006
#define MYIP_FLYBYWIRE_PORT_2       3007
#define MYIP_FLYBYWIRE_PORT_3       3008
#define MYIP_MAVLINK_PORT_1         3009
#define MYIP_MAVLINK_PORT_2         3010
#define MYIP_MAVLINK_PORT_3         3011
#define MYIP_DEBUG_PORT_1           3012
#define MYIP_DEBUG_PORT_2           3013
#define MYIP_DEBUG_PORT_3           3014
#define MYIP_ADSB_PORT_1            3015
#define MYIP_ADSB_PORT_2            3016
#define MYIP_ADSB_PORT_3            3017
#define MYIP_LOGO_PORT_1            3018
#define MYIP_LOGO_PORT_2            3019
#define MYIP_LOGO_PORT_3            3020
#define MYIP_CAM_TRACKING_PORT_1    3021
#define MYIP_CAM_TRACKING_PORT_2    3022
#define MYIP_CAM_TRACKING_PORT_3    3023
#define MYIP_GPSTEST_PORT_1         3024
#define MYIP_GPSTEST_PORT_2         3025
#define MYIP_GPSTEST_PORT_3         3026
#define MYIP_PWMREPORT_PORT_1       3027
#define MYIP_PWMREPORT_PORT_2       3028
#define MYIP_PWMREPORT_PORT_3       3029

//Common port numbers for other, 3rd party software
#define MYIP_QGROUND_CONTROL_UDP_PORT   14550
#define MYIP_TELNET_PORT                23



#endif	/* MYIPKNOWNHOSTS_H */

