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

#include "udp_client.h"

int sendData(char *_stServerName, int _iPort, double _dblData)
{
   int sock, length, n;
   struct sockaddr_in server, from;
   struct hostent *hp;

   double buffer[1];

   sock = socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0)
   {
       //error("socket");
   }

   server.sin_family = AF_INET;
   hp = gethostbyname(_stServerName);

   memcpy((char *)&server.sin_addr, (char *)hp->h_addr, hp->h_length);
   server.sin_port = htons(_iPort);
   length=sizeof(struct sockaddr_in);

   n=sendto(sock, &_dblData, sizeof(double), 0, &server, length);
   printf("sending %f, n=%d\n", _dblData, n);

   if (n < 0)
   {
       //error("Sendto");
   }
}
