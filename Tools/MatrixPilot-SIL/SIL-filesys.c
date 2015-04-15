//
//  SIL-udb.c
//  MatrixPilot-SIL
//
//  Created by Ben Levitt on 2/1/13.
//  Copyright (c) 2013 MatrixPilot. All rights reserved.
//

#if (WIN == 1 || NIX == 1)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../libUDB/libUDB.h"
#include "SIL-config.h"


int filesys_init(void)
{
	return 1;
}


#endif // (WIN == 1 || NIX == 1)
