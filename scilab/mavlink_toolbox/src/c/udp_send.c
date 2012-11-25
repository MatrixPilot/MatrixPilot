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

#include "scicos_block4.h"
#include "udp_client.h"

void udp_send(scicos_block *block,int flag)
{
    int i = 0;
    double *y = GetRealInPortPtrs(block,1);
    int *iPar = GetIparPtrs(block);
    int *piAddress = GetOparPtrs(block, 1);

    char *pstHostname = NULL;

    getAllocatedSingleString(NULL, piAddress, &pstHostname);

    switch(flag) {
    case DerivativeState:
    {
        printf("[DEBUG] udp_send :: DerivativeState\n");
    }
    break;
    case OutputUpdate:
    {
        printf("[DEBUG] udp_send :: OutputUpdate\n");
        printf("[DEBUG] portNumber = %d\n", *iPar);
        printf("[DEBUG] hostName = %s\n", pstHostname);

        sendData(pstHostname, *iPar, y[0]);
    }
    break;
    case StateUpdate:
    {
        // send data to UDP (can block)
        printf("[DEBUG] udp_send :: StateUpdate\n");
    }
    break;
    case OutputEventTiming:
    {
        printf("[DEBUG] udp_send :: OutputEventTiming\n");
    }
    break;
    case Initialization:
    {
        printf("[DEBUG] udp_send :: Initialization\n");
        // initialise the connection
        // use block->work to store any internal state
    }
    break;
    case Ending:
    {
        printf("[DEBUG] udp_send :: Ending\n");
        // close the connection
    }
    break;
    case ReInitialization:
    {
        printf("[DEBUG] udp_send :: ReInitialization\n");
    }
    break;
    default:
        printf("[DEBUG] udp_send :: default ?????\n");
        break;
    }
}
