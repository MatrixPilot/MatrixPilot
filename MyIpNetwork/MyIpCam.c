#ifndef _MYIPCAM_C_
#define _MYIPCAM_C_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#if (NETWORK_USE_CAM == 1)
#if (CAM_USE_EXTERNAL_TARGET_DATA == 1)

#include "TCPIP_Stack/TCPIP.h"
#include "defines.h"
#include "MyIpData.h"
#include "MyIpCam.h"
#include "euler_angles.h"

//////////////////////////
// Module Variables
char MyIp_cam_high_byte;
unsigned char MyIp_cam_checksum;


void MyIpsio_cam_newMsg(unsigned char);
void MyIpsio_cam_data( unsigned char inchar ) ;
void MyIpsio_cam_checksum( unsigned char inchar ) ;

void (* MyIpsio_cam_parse ) ( unsigned char inchar ) = &MyIpsio_cam_newMsg ;


void MyIpOnConnect_Cam(BYTE s) {
    // Print any one-time connection annoucement text
    LoadStringSocket(s, "\r\nYou've connected to Cam on "); // 33 chars
    LoadStringSocket(s, ID_LEAD_PILOT); // 15ish chars
    LoadStringSocket(s, "'s aircraft. More info at "); // 26 chars
    LoadStringSocket(s, ID_DIY_DRONES_URL); // 45ish chars
    LoadStringSocket(s, "\r\n"); // 2 chars
    MyIpData[s].sendPacket = TRUE; // send right away
}

void MyIpInit_Cam(BYTE s) {
    // This gets called once for every socket we're configured to use for this module.
}

void MyIpService_Cam(BYTE s) {
    
}

BOOL MyIpThreadSafeSendPacketCheck_Cam(BYTE s, BOOL doClearFlag) {
    // since this data comes from, and goes to, the idle thread we
    // don't need to deal with any thread issues
    BOOL sendpacket = MyIpData[s].sendPacket;
    if (doClearFlag) {
        MyIpData[s].sendPacket = FALSE;
    }
    return sendpacket;
}

int MyIpThreadSafeReadBufferHead_Cam(BYTE s) {
    // since this data comes from, and goes to, the idle thread we
    //  don't need to deal with any thread issues
    return MyIpData[s].buffer_head;
}

void MyIpProcessRxData_Cam(BYTE s) {
    BYTE rxchar;
    BOOL successfulRead;

    do {
        if (eTCP == MyIpData[s].type) {
            successfulRead = TCPGet(MyIpData[s].socket, &rxchar);
        } else //if (eUDP == MyIpData[s].type)
        {
            successfulRead = UDPGet(&rxchar);
        }

        if (successfulRead)
        {
            (* MyIpsio_cam_parse) ( rxchar ) ; // parse the input byte
        }
    } while (successfulRead);
}

char MyIp_cam_hex_char_val(unsigned char inchar)
{
	if (inchar >= '0' && inchar <= '9')
	{
		return (inchar - '0') ;
	}
	else if (inchar >= 'A' && inchar <= 'F')
	{
		return (inchar - 'A' + 10) ;
	}
	return -1 ;
}
void MyIpsio_cam_newMsg( unsigned char inchar )
{
    if (inchar == 'T')
    {
        MyIp_cam_high_byte = -1 ; // -1 means we don't have the high byte yet (0-15 means we do)
        MyIp_cam_checksum = 0 ;
        MyIpsio_cam_parse = &MyIpsio_cam_data ;
        camera_live_begin() ;
    }
}
void MyIpsio_cam_data( unsigned char inchar )
{
	if (inchar == '*')
	{
		MyIp_cam_high_byte = -1 ;
		MyIpsio_cam_parse = &MyIpsio_cam_checksum ;
	}
	else
	{
		char hexVal = MyIp_cam_hex_char_val(inchar) ;
		if (hexVal == -1)
		{
			MyIpsio_cam_parse = &MyIpsio_cam_newMsg ;
			return ;
		}
		else if (MyIp_cam_high_byte == -1)
		{
			MyIp_cam_high_byte = hexVal * 16 ;
		}
		else
		{
			unsigned char combined = MyIp_cam_high_byte + hexVal ;
			flightplan_live_received_byte(MyIp_cam_high_byte + hexVal) ;
			camera_live_received_byte(combined) ;
			MyIp_cam_high_byte = -1 ;
		}
		MyIp_cam_checksum += inchar ;
	}
}


void MyIpsio_cam_checksum( unsigned char inchar )
{
	char hexVal = MyIp_cam_hex_char_val(inchar) ;
	if (hexVal == -1)
	{
		MyIpsio_cam_parse = &MyIpsio_cam_newMsg ;
	}
	else if (MyIp_cam_high_byte == -1)
	{
		MyIp_cam_high_byte = hexVal * 16 ;
	}
	else
	{
		unsigned char v = MyIp_cam_high_byte + hexVal ;
		if (v == MyIp_cam_checksum)
		{
			flightplan_live_commit() ;
		}
		MyIpsio_cam_parse = &MyIpsio_cam_newMsg ;
	}
}


#endif // (CAM_USE_EXTERNAL_TARGET_DATA == 1)
#endif // (NETWORK_USE_CAM == 1)
#endif // ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPCAM_C_

