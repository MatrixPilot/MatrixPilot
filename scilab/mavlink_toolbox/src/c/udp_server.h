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

#ifndef __UDP_SERVER_H__
#define __UDP_SERVER_H__

#include <sys/types.h>

#ifndef _MSC_VER
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#else
#include <WinSock.h>
#endif

#define ERROR_CAN_T_OPEN_SOCKET         -1
#define ERROR_CAN_T_BIND                -2
#define ERROR_RECVFROM                  -3
#define ERROR_SENDTO                    -4


int startServer(int _iPort);

closeServer(int sock);

int getData(int _iSocket, char* pBuff, int maxSize);

#endif /* !__UDP_SERVER_H__ */
