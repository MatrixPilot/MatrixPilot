/*********************************************************************
 *
 *      Berekely Socket Distribution API Header File
 *
 *********************************************************************
 * FileName:        BerkeleyAPI.h
 * Description:     Berkeley socket Distribution(BSD) APIs for Microchip TCPIP Stack
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.30 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2009 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 * Author               Date    	Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Aseem Swalah         4/3/08  	Original
 ********************************************************************/

#ifndef _BERKELEY_API_HEADER_FILE
#define _BERKELEY_API_HEADER_FILE

typedef BYTE SOCKET;   //Socket descriptor

#define AF_INET         2			// Internet Address Family - UDP, TCP, etc.

#define IP_ADDR_ANY     0u			// IP Address for server binding
#define INADDR_ANY      0x00000000u	// IP address for server binding.


#define SOCK_STREAM 100  //Connection based byte streams. Use TCP for the internet address family.
#define SOCK_DGRAM  110  //Connectionless datagram socket. Use UDP for the internet address family.
 
#define IPPROTO_IP      0   // Indicates IP pseudo-protocol.
#define IPPROTO_TCP     6   // Indicates TCP for the internet address family.
#define IPPROTO_UDP     17  // Indicates UDP for the internet address family.

#define SOCKET_ERROR            (-1) //Socket error
#define SOCKET_CNXN_IN_PROGRESS (-2) //Socket connection state.
#define SOCKET_DISCONNECTED     (-3) //Socket disconnected

typedef enum
{
    SKT_CLOSED,   			// Socket closed state indicating a free descriptor
    SKT_CREATED, 			// Socket created state for TCP and UDP sockets
    SKT_BOUND,   			// Socket bound state for TCP and UDP sockets
    SKT_BSD_LISTEN,			// Listening state for TCP BSD listener handle "socket"
    SKT_LISTEN,  			// TCP server listen state
    SKT_IN_PROGRESS, 		// TCP client connection in progress state
    SKT_EST,  				// TCP client or server established state
    SKT_DISCONNECTED		// TCP client or server no longer connected to the remote host (but was historically)
} BSD_SCK_STATE; // Berkeley Socket (BSD) states

struct BSDSocket
{
    int            SocketType; // Socket type
    BSD_SCK_STATE  bsdState; //Socket state
    WORD           localPort; //local port
    WORD           remotePort; //remote port
    DWORD          remoteIP; //remote IP
    int            backlog; // maximum number or client connection
    BOOL           isServer; // server/client check
    TCP_SOCKET     SocketID; // Socket ID
}; // Berkeley Socket structure

#define INVALID_TCP_PORT   (0L)  //Invalide TCP port

struct in_addr
{
    union
   {
       struct { BYTE s_b1,s_b2,s_b3,s_b4; } S_un_b; // IP address in Byte
       struct { WORD s_w1,s_w2; } S_un_w; //IP address in Word
       DWORD S_addr; //IP address
   }S_un; //union of IP address
    
#define s_addr  S_un.S_addr //can be used for most tcp & ip code
#define s_host  S_un.S_un_b.s_b2 //host on imp
#define s_net   S_un.S_un_b.s_b1 // network
#define s_imp   S_un.S_un_w.s_w2 // imp
#define s_impno S_un.S_un_b.s_b4 // imp number
#define s_lh    S_un.S_un_b.s_b3 // logical host
}; // in_addr structure

struct __attribute__((__packed__)) sockaddr
{
    unsigned short   sa_family;   //address family
    char    sa_data[14];       //up to 14 bytes of direct address
}; //generic address structure for all address families

struct __attribute__((__packed__)) sockaddr_in
{
    short   sin_family; //Address family; must be AF_INET.
    WORD    sin_port;  //Internet Protocol (IP) port.
    struct  in_addr sin_addr; //IP address in network byte order.
    char    sin_zero[8];  //Padding to make structure the same size as SOCKADDR. 
}; //In the Internet address family

typedef struct sockaddr_in SOCKADDR_IN; //In the Internet address family
typedef struct sockaddr SOCKADDR;  // generic address structure for all address families

void BerkeleySocketInit(void);
SOCKET socket( int af, int type, int protocol );
int bind( SOCKET s, const struct sockaddr* name, int namelen );
int listen( SOCKET s, int backlog );
SOCKET accept( SOCKET s, struct sockaddr* addr, int* addrlen );
int connect( SOCKET s, struct sockaddr* name, int namelen );
int send( SOCKET s, const char* buf, int len, int flags );
int sendto( SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen );
int recv( SOCKET s, char* buf, int len, int flags );
int recvfrom( SOCKET s, char* buf, int len, int flags, struct sockaddr* from, int* fromlen );
int gethostname(char* name, int namelen);
int closesocket( SOCKET s );

#endif

