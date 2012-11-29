/*
 *  Scilab ( http://www.scilab.org/ ) - This file is part of Scilab
 *  Copyright (C) 2011-2011 - DIGITEO - Bruno JOFRET
 *
 *  This file must be used under the terms of the CeCILL.
 *  This source file is licensed as described in the file COPYING, which
 *  you should have received as part of this distribution.  The terms
 *  are also available at
 *  http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 */

#include <stdio.h>

#include "udp_server.h"
#include <fcntl.h>

/*
** Start Server
*/
int startServer(int _iPort)
{
    struct sockaddr_in server;
    struct sockaddr_in from;
    int sock = 0;
    int iFromLength = 0;
    int iServerLength = 0;

    double pdblData[1];

    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0)
    {
        return ERROR_CAN_T_OPEN_SOCKET;
    }

    server.sin_family=AF_INET;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=htons(_iPort);

    iServerLength = sizeof(server);

    if (bind(sock, (struct sockaddr *)&server, iServerLength) < 0)
    {
        return ERROR_CAN_T_BIND;
    }

//#ifndef _MSC_VER
//
//      int nonBlocking = 1;
//      if ( fcntl( sock, F_SETFL, O_NONBLOCK, nonBlocking ) == -1 )
//      {
//          printf( "failed to set non-blocking socket\n" );
//          return ERROR_CAN_T_BIND;
//      }
//
//#else
//
//      DWORD nonBlocking = 1;
//      if ( ioctlsocket( sock, FIONBIO, &nonBlocking ) != 0 )
//      {
//          printf( "failed to set non-blocking socket\n" );
//          return false;
//      }
//
//#endif


    int val = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val) );

//    struct linger {
//        int l_onoff;    /* linger active */
//        int l_linger;   /* how many seconds to linger for */
//    } ling;
//
//    ling.l_onoff = 1;
//    ling.l_linger = 5;
//
//    setsockopt(sock, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling) );

    struct timeval tv;

    tv.tv_sec = 5;  /* 30 Secs Timeout */
    tv.tv_usec = 0;  // Not init'ing this can cause strange errors

    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv,sizeof(struct timeval));

    return sock;
}

closeServer(int sock)
{
#ifndef _MSC_VER
//    shutdown(sock, SHUT_RDWR);
    close(sock);
#else
    //closesocket(sock);
    //shutdown(sock, SHUT_RDWR);
    close(sock);
#endif
}

int getData(int _iSocket, char* pBuff, int maxSize)
{
    struct sockaddr_in from;
    int iFromLength = 0;
    int intData = 0;
    int iByteReceive = 0;

    iFromLength = sizeof(struct sockaddr_in);

    iByteReceive = recvfrom(_iSocket, pBuff, maxSize, 0, (struct sockaddr *)&from, &iFromLength);

    printf("received %i, iByteReceive=%d\n", intData, iByteReceive);

    if(iByteReceive < 0) return 0;
    return iByteReceive;


    //iByteReceive = sendto(_iSocket, iByteReceive, sizeof(iByteReceive), 0, (struct sockaddr *)&from, iFromLength);
    //if (iByteReceive < 0)
    //{
    //    return ERROR_SENDTO;
    //}
}

