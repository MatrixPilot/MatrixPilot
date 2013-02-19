#ifndef _MYIPCAM_TRACKING_C_
#define _MYIPCAM_TRACKING_C_

#include "defines.h"
#if (USE_NETWORK == 1) && (NETWORK_USE_CAM_TRACKING == 1) && (CAM_USE_EXTERNAL_TARGET_DATA == 1)

#include "TCPIP_Stack/TCPIP.h"
#include "MyIpData.h"
#include "MyIpCam.h"
#include "euler_angles.h"
#include "MyIpHelpers.h"

//////////////////////////
// Module Variables
char MyIp_cam_high_byte;
unsigned char MyIp_cam_checksum;


void MyIpsio_cam_newMsg(unsigned char);
void MyIpsio_cam_data( unsigned char inchar ) ;
void MyIpsio_cam_checksum( unsigned char inchar ) ;

void (* MyIpsio_cam_parse ) ( unsigned char inchar ) = &MyIpsio_cam_newMsg ;


void MyIpOnConnect_CamTracking(BYTE s) {
    // Print any one-time connection annoucement text
    StringToSocket(s, "\r\nYou've connected to CamTracking on "); // 33 chars
    StringToSocket(s, ID_LEAD_PILOT); // 15ish chars
    StringToSocket(s, "'s aircraft. More info at "); // 26 chars
    StringToSocket(s, ID_DIY_DRONES_URL); // 45ish chars
    StringToSocket(s, "\r\n"); // 2 chars
    MyIpData[s].sendPacket = TRUE; // send right away
}

void MyIpInit_CamTracking(BYTE s) {
    // This gets called once for every socket we're configured to use for this module.
}

void MyIpService_CamTracking(BYTE s) {
    
}

BOOL MyIpThreadSafeSendPacketCheck_CamTracking(BYTE s, BOOL doClearFlag) {
    // since this data comes from, and goes to, the idle thread we
    // don't need to deal with any thread issues
    BOOL sendpacket = MyIpData[s].sendPacket;
    if (doClearFlag) {
        MyIpData[s].sendPacket = FALSE;
    }
    return sendpacket;
}

int MyIpThreadSafeReadBufferHead_CamTracking(BYTE s) {
    // since this data comes from, and goes to, the idle thread we
    //  don't need to deal with any thread issues
    return MyIpData[s].buffer_head;
}

void MyIpProcessRxData_CamTracking(BYTE s) {
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
		char hexVal = MyIphex_char_val(inchar) ;
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
	char hexVal = MyIphex_char_val(inchar) ;
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


#endif // #if (USE_NETWORK == 1)
#endif // _MYIPCAM_TRACKING_C_

