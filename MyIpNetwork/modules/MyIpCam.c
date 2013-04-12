#ifndef _MYIPCAM_TRACKING_C_
#define _MYIPCAM_TRACKING_C_

#include "defines.h"
#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_CAM_TRACKING == 1) && (CAM_USE_EXTERNAL_TARGET_DATA == 1)

#include "TCPIP_Stack/TCPIP.h"
#include "MyIpData.h"
#include "MyIpCam.h"
#include "euler_angles.h"
#include "MyIpHelpers.h"

//////////////////////////
// Module Variables
#if 0
char MyIp_cam_high_byte;
unsigned char MyIp_cam_checksum;
void MyIpsio_cam_newMsg(unsigned char);
void MyIpsio_cam_data( unsigned char inchar ) ;
void MyIpsio_cam_checksum( unsigned char inchar ) ;
void (* MyIpsio_cam_parse ) ( unsigned char inchar ) = &MyIpsio_cam_newMsg ;
#endif

#define CAM_TRACKING_PACKET_MAX_LENGTH 60
BYTE RxCSVbufCam[MAX_NUM_INSTANCES_OF_MODULES][CAM_TRACKING_PACKET_MAX_LENGTH];
int RxCSVbufCamIndex[MAX_NUM_INSTANCES_OF_MODULES];
void parseCamPacket(const uint8_t* bufCSV, const int16_t len);


void MyIpOnConnect_CamTracking(uint8_t s) {
    // Print any one-time connection annoucement text
    StringToSocket(s, "\r\nYou've connected to CamTracking on "); // 33 chars
    StringToSocket(s, ID_LEAD_PILOT); // 15ish chars
    StringToSocket(s, "'s aircraft. More info at "); // 26 chars
    StringToSocket(s, ID_DIY_DRONES_URL); // 45ish chars
    StringToSocket(s, "\r\n"); // 2 chars
    MyIpData[s].sendPacket = TRUE; // send right away

    uint8_t si = MyIpData[s].instance;
    RxCSVbufCamIndex[si] = 0;
}

void MyIpInit_CamTracking(uint8_t s) {
    // This gets called once for every socket we're configured to use for this module.
}

void MyIpService_CamTracking(uint8_t s) {
    
}

boolean MyIpThreadSafeSendPacketCheck_CamTracking(uint8_t s, boolean doClearFlag) {
    // since this data comes from, and goes to, the idle thread we
    // don't need to deal with any thread issues
    boolean sendpacket = MyIpData[s].sendPacket;
    if (doClearFlag) {
        MyIpData[s].sendPacket = FALSE;
    }
    return sendpacket;
}

int16_t MyIpThreadSafeReadBufferHead_CamTracking(uint8_t s) {
    // since this data comes from, and goes to, the idle thread we
    //  don't need to deal with any thread issues
    return MyIpData[s].buffer_head;
}

void MyIpProcessRxData_CamTracking(uint8_t s)
{
    uint8_t si = MyIpData[s].instance;

    if (eTCP == MyIpData[s].type)
    {
        while (TCPIsGetReady(MyIpData[s].socket))
        {
            int16_t index = RxCSVbufCamIndex[si];
            TCPGet(MyIpData[s].socket, &RxCSVbufCam[si][index]);
            //TCPPut(MyIpData[s].socket, RxCSVbufCam[si][index]); // ECHO

            if ((RxCSVbufCam[si][index] == '\r') ||
                (RxCSVbufCam[si][index] == '\n') ||
                ((index+1) >= CAM_TRACKING_PACKET_MAX_LENGTH))
            {
                RxCSVbufCam[si][index] = ',';
                parseCamPacket(RxCSVbufCam[si],index+1);
                RxCSVbufCamIndex[si] = 0;
            }
            else
              RxCSVbufCamIndex[si]++;
        }
    }
    else //if (eUDP == MyIpData[s].type)
    {
        while (UDPIsGetReady(MyIpData[s].socket))
        {
            uint8_t rxData;
            UDPGet(&rxData);
            //UDPGetArray(buf, CAM_TRACKING_PACKET_MAX_LENGTH);
            //parseGpsSpoofPacket(buf);
        }
    }
}

void parseCamPacket(const uint8_t* bufCSV, const int16_t len)
{
    #define CAM_PARAM_LENGTH (4)
    int32_t camData[CAM_PARAM_LENGTH+1]; // +1 just in case becaue I havn't tested the CSV parser enough
    uint8_t parseCount;
    int16_t i;

    for (i=0;i<CAM_PARAM_LENGTH;i++)
        camData[i] = 0;

    parseCount = parseCSV(bufCSV, len, camData, CAM_PARAM_LENGTH);
    if (parseCount >= CAM_PARAM_LENGTH)
    {
        struct relative3D target;

        // header camData[0] is ignored
        target.x = camData[1];
        target.y = camData[2];
        target.z = camData[3];
        camera_live_commit_values(target);
    }
}

#if 0
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


void MyIpsio_cam_checksum(uint8_t inchar )
{
	int8_t hexVal = MyIphex_char_val(inchar) ;
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
		//if (v == MyIp_cam_checksum)
		{
                    // when over IP, checksums are taken care of ata lower level
			flightplan_live_commit() ;
		}
		MyIpsio_cam_parse = &MyIpsio_cam_newMsg ;
	}
}
#endif

#endif // #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE)
#endif // _MYIPCAM_TRACKING_C_

