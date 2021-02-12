#include "stdafx.h"
#include "SendUBX.h"
#include "SerialIO.h"
#include "utility.h"

// At 40hz, this message will be ~6500bps

// 20 bytes
unsigned char NAV_BODYRATES[] = {
            0xB5, 0x62,     // Header
            0x01, 0xAB,     // ID
            0x0C, 0x00,     // Payload Length
            0x00, 0x00,     // P
            0x00, 0x00,     // Q
            0x00, 0x00,     // R
            0x00, 0x00,     // gravity minus acceleration, UDBx
            0x00, 0x00,     // gravity minus acceleration, UDBy
            0x00, 0x00,     // gravity minus acceleration, UDBz
            0x00, 0x00      // Checksum
            };

unsigned char NAV_KEYSTROKE[] = {
            0xB5, 0x62,     // Header
            0x01, 0xAC,     // ID
            0x02, 0x00,     // Payload Length
            0x00, 0x00,     // KeySniffer keystroke data, ckey, vkey
            0x00, 0x00      // Checksum
            };

// At 4hz the normal messages are ~6000 bps

// 60 bytes
unsigned char NAV_SOL[] = {
            0xB5, 0x62,             // Header           - DONE
            0x01, 0x06,             // ID               - DONE
            0x34, 0x00,             // Payload Length   - DONE
            0x00, 0x00, 0x00, 0x00, // iTOW             - DONE
            0x00, 0x00, 0x00, 0x00, // fTOW             - DONE
            0x00, 0x00,             // week             - DONE
            0x03,                   // gpsFix           - DONE
            0x0D,                   // flags            - DONE
            0x00, 0x00, 0x00, 0x00, // ecefX            - DONE
            0x00, 0x00, 0x00, 0x00, // ecefY            - DONE
            0x00, 0x00, 0x00, 0x00, // ecefZ            - DONE
            0x64, 0x00, 0x00, 0x00, // pAcc             - DONE
            0x00, 0x00, 0x00, 0x00, // ecefVX           - DONE
            0x00, 0x00, 0x00, 0x00, // ecefVY           - DONE
            0x00, 0x00, 0x00, 0x00, // ecefVZ           - DONE
            0x64, 0x00, 0x00, 0x00, // sACC             - DONE
            0x64, 0x00,             // pDOP             - DONE
            0x00,                   // res1             - DONE
            0x04,                   // numSV            - DONE
            0x00, 0x00, 0x00, 0x00, // res2             - DONE
            0x00, 0x00              // Checksum         - DONE
            };

// 26 bytes
unsigned char NAV_DOP[] = {
            0xB5, 0x62,             // Header           - DONE
            0x01, 0x04,             // ID               - DONE
            0x12, 0x00,             // Payload Length   - DONE
            0x00, 0x00, 0x00, 0x00, // iTOW             - DONE
            0x64, 0x00,             // gDOP             - DONE
            0x64, 0x00,             // pDOP             - DONE
            0x64, 0x00,             // tDOP             - DONE
            0x64, 0x00,             // vDOP             - DONE
            0x64, 0x00,             // hDOP             - DONE
            0x64, 0x00,             // nDOP             - DONE
            0x64, 0x00,             // eDOP             - DONE
            0x00, 0x00              // Checksum         - DONE
            };

// 36 bytes
unsigned char NAV_POSLLH[] = {
            0xB5, 0x62,             // Header           - DONE
            0x01, 0x02,             // ID               - DONE
            0x1C, 0x00,             // Payload Length   - DONE
            0x00, 0x00, 0x00, 0x00, // iTOW             - DONE
            0x00, 0x00, 0x00, 0x00, // lon              - DONE
            0x00, 0x00, 0x00, 0x00, // lat              - DONE
            0x00, 0x00, 0x00, 0x00, // height           - DONE
            0x00, 0x00, 0x00, 0x00, // hMSL             - DONE
            0xE8, 0x03, 0x00, 0x00, // hAcc             - DONE
            0xE8, 0x03, 0x00, 0x00, // vAcc             - DONE
            0x00, 0x00              // Checksum         - DONE
            };

// 44 bytes
unsigned char NAV_VELNED[] = {
            0xB5, 0x62,             // Header           - DONE
            0x01, 0x12,             // ID               - DONE
            0x24, 0x00,             // Payload Length   - DONE
            0x00, 0x00, 0x00, 0x00, // iTOW             - DONE
            0x00, 0x00, 0x00, 0x00, // velN             - DONE
            0x00, 0x00, 0x00, 0x00, // velE             - DONE
            0x00, 0x00, 0x00, 0x00, // velD             - DONE
            0x00, 0x00, 0x00, 0x00, // speed            - DONE
            0x00, 0x00, 0x00, 0x00, // gSpeed           - DONE
            0x00, 0x00, 0x00, 0x00, // heading          - DONE
            0x64, 0x00, 0x00, 0x00, // sAcc             - DONE
            0xA0, 0x86, 0x01, 0x00, // cAcc             - DONE
            0x00, 0x00              // Checksum         - DONE
            };


static void CalculateChecksum(unsigned char* msg)
{
	// length is technically 2 bytes (in little endian order) but we're 
	// not using any messages over 256 bytes long, so we can get away 
	// with just looking at the first byte.

	int length = msg[4];
	int i;
	int CK_A = 0;
	int CK_B = 0;
	for (i = 2; i < (length + 6); i++) {
		CK_A += msg[i];
		CK_B += CK_A;
	}
	msg[length + 6] = CK_A;
	msg[length + 7] = CK_B;
}

static void Store4LE(unsigned char* store, union longbbbb data)
{
	store[0] = data.__.B0;
	store[1] = data.__.B1;
	store[2] = data.__.B2;
	store[3] = data.__.B3;
}

static void Store2LE(unsigned char* store, union intbb data)
{
	store[0] = data._.B0;
	store[1] = data._.B1;
}

////////////////////////////////////////////////////////////////////////////////

void Send_NAV_KEYSTROKE(int inFlags, char inVirtualKey)
{
	NAV_KEYSTROKE[6] = (unsigned char)inFlags;
	NAV_KEYSTROKE[7] = (unsigned char)inVirtualKey;

	CalculateChecksum(NAV_KEYSTROKE);
	SendToComPort(sizeof(NAV_KEYSTROKE), NAV_KEYSTROKE);
}

void Send_NAV_BODYRATES(float P_plane, float Q_plane, float R_plane,
                        float X_accel, float Y_accel, float Z_accel)
{
	union intbb Temp2;

	// Angular rate
	// multiply by 5632 (constant from UDB code)
	// Divide by SCALEGYRO(3.0 for red board)
	// 1 * 5632 / 3.0 = 1877.33
	Temp2.BB = (int)(P_plane * 1877.33);
	Store2LE(&NAV_BODYRATES[6], Temp2);
	Temp2.BB = (int)(Q_plane * 1877.33);
	Store2LE(&NAV_BODYRATES[8], Temp2);
	Temp2.BB = (int)(R_plane * 1877.33);
	Store2LE(&NAV_BODYRATES[10], Temp2);

	// Lastly we need to convert from X-Plane units (m/s^2) to the arbitrary units used by the UDB

	// Accelerations are in m/s^2
	// Divide by 9.8 to get g's
	// Multiply by 5280 (constant from UDB code)
	// Divide by SCALEACCEL (2.64 for red board)
	// 1 / 9.8 * 5280 / 2.64 = 204.8
	Temp2.BB = (int)(X_accel * 204.8);
	Store2LE(&NAV_BODYRATES[12], Temp2);
	Temp2.BB = (int)(Y_accel * 204.8);
	Store2LE(&NAV_BODYRATES[14], Temp2);
	Temp2.BB = (int)(Z_accel * 204.8);
	Store2LE(&NAV_BODYRATES[16], Temp2);

	CalculateChecksum(NAV_BODYRATES);
	SendToComPort(sizeof(NAV_BODYRATES), NAV_BODYRATES);
}

void Send_NAV_VELNED(int LocalSecsInt,
                     double local_vx, double local_vy, double local_vz,
                     double air_speed, double speed_over_ground,
                     double course_over_ground)
{
	union longbbbb Temp4;

	Temp4.WW = LocalSecsInt;
	Store4LE(&NAV_VELNED[6], Temp4);

	Temp4.WW = (int)(local_vx * 100);
	Store4LE(&NAV_VELNED[14], Temp4);
	Temp4.WW = (int)(local_vy * -100);
	Store4LE(&NAV_VELNED[18], Temp4);
	Temp4.WW = (int)(local_vz * -100);
	Store4LE(&NAV_VELNED[10], Temp4);
	Temp4.WW = (int)(air_speed * 100);
	Store4LE(&NAV_VELNED[22], Temp4);
	Temp4.WW = (int)speed_over_ground;
	Store4LE(&NAV_VELNED[26], Temp4);
	Temp4.WW = (int)(100000.0 * course_over_ground);
	Store4LE(&NAV_VELNED[30], Temp4);

	CalculateChecksum(NAV_VELNED);
	SendToComPort(sizeof(NAV_VELNED), NAV_VELNED);
}

void Send_NAV_POSLLH(int LocalSecsInt,
                     double latitude, double longitude, double elevation)
{
	union longbbbb Temp4;

	Temp4.WW = LocalSecsInt;
	Store4LE(&NAV_POSLLH[6], Temp4);
	Temp4.WW = (int)(latitude * 10000000);
	Store4LE(&NAV_POSLLH[14], Temp4);
	Temp4.WW = (int)(longitude * 10000000);
	Store4LE(&NAV_POSLLH[10], Temp4);
	Temp4.WW = (int)(elevation * 1000);
	Store4LE(&NAV_POSLLH[18], Temp4);
	Store4LE(&NAV_POSLLH[22], Temp4);

	CalculateChecksum(NAV_POSLLH);
	SendToComPort(sizeof(NAV_POSLLH), NAV_POSLLH);
}

void Send_NAV_DOP(int LocalSecsInt)
{
	union longbbbb Temp4;

	Temp4.WW = LocalSecsInt;
	Store4LE(&NAV_DOP[6], Temp4);

	CalculateChecksum(NAV_DOP);
	SendToComPort(sizeof(NAV_DOP), NAV_DOP);
}

void Send_NAV_SOL(int LocalSecsInt, float LocalSecsFloat, int Week, 
                  double local_x,  double local_y,  double local_z, 
                  double local_vx, double local_vy, double local_vz,
                  float mag_x, float mag_y, float mag_z)
{
	union longbbbb Temp4;
	union intbb Temp2;

	Temp4.WW = LocalSecsInt;
	Store4LE(&NAV_SOL[6], Temp4);
	Temp4.WW = (int)LocalSecsFloat;
	Store4LE(&NAV_SOL[10], Temp4);
	Temp2.BB = Week;
	Store2LE(&NAV_SOL[14], Temp2);

	Temp4.WW = (int)(local_x * 100);
	Store4LE(&NAV_SOL[18], Temp4);
	Temp4.WW = (int)(local_y * 100);
	Store4LE(&NAV_SOL[22], Temp4);
	Temp4.WW = (int)(local_z * 100);
	Store4LE(&NAV_SOL[26], Temp4);
	Temp4.WW = (int)(local_vx * 100);
	Store4LE(&NAV_SOL[34], Temp4);
	Temp4.WW = (int)(local_vy * 100);
	Store4LE(&NAV_SOL[38], Temp4);
	Temp4.WW = (int)(local_vz * 100);
	Store4LE(&NAV_SOL[42], Temp4);

	// store the magnetometer data in slots 30, 32 and 46 of NAV_SOL message
	// these slots are used by Ublox, but not by HILSIM
	// beware, converting from NED to UDB body frame - swap x & y, invert y (or is it x?)
	Temp2.BB = (int)-mag_y;
	Store2LE(&NAV_SOL[30], Temp2);
	Temp2.BB = (int)mag_x;
	Store2LE(&NAV_SOL[32], Temp2);
	Temp2.BB = (int)mag_z;
	Store2LE(&NAV_SOL[46], Temp2);

	CalculateChecksum(NAV_SOL);
	SendToComPort(sizeof(NAV_SOL), NAV_SOL);
}
