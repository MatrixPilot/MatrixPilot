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
#include "udp_server.h"

static int iSocket = 0;

void mavlink_open(scicos_block *block, int flag)
{
}

void mavlink_receive(scicos_block *block, int flag)
{
    double *y = GetRealOutPortPtrs(block,1);
    int *piPort = GetIparPtrs(block);

	switch(flag) {
    case DerivativeState:
    {
        printf("[DEBUG] mavlink_receive :: DerivativeState\n");
    }
    break;
    case OutputUpdate:
    {
        printf("[DEBUG] udp_receive :: OutputUpdate\n");
        // receive data from UDP (can block)
        y[0] = getData(iSocket);
    }
    break;
    case StateUpdate:
    {
        printf("[DEBUG] udp_receive :: StateUpdate\n");
    }
    break;
    case OutputEventTiming:
    {
        printf("[DEBUG] mavlink_receive :: OutputEventTiming\n");
        // can emit output event (not supported yet)
    }
    break;
    case Initialization:
    {
        printf("[DEBUG] mavlink_receive :: Initialization\n");
        // initialise the connection
        // use block->work to store any internal state
        iSocket = startServer(*piPort);
    }
    break;
    case Ending:
    {
        printf("[DEBUG] mavlink_receive :: Ending\n");
        // close the connection
    }
    break;
    case ReInitialization:
    {
        printf("[DEBUG] mavlink_receive :: ReInitialization\n");
    }
    break;
    default:
        printf("[DEBUG] mavlink_receive :: default ?????\n");
		break;
	}
}
