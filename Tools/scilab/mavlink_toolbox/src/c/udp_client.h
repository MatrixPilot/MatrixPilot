/*
 *  Scilab ( http://www.scilab.org/ ) - This file is part of Scilab
 *  Copyright (C) 2011-2012 - Scilab Enterprises - Bruno JOFRET
 *
 *  This file must be used under the terms of the CeCILL.
 *  This source file is licensed as described in the file COPYING, which
 *  you should have received as part of this distribution.  The terms
 *  are also available at
 *  http://www.cecill.info/licences/Licence_CeCILL_V2-en.txt
 *
 */

#ifndef __UDP_CLIENT_H__
#define __UDP_CLIENT_H__

#include <sys/types.h>

#ifndef _MSC_VER
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#else

#include <WinSock.h>

#endif

int sendData(char *_stServerName, int _iPort, double _dblData);



#endif /* !__UDP_CLIENT_H__ */
