//
//  SIL-filesystem.c
//  MatrixPilot-SIL
//
//  Created by Ben Levitt on 2/10/13.
//  Copyright (c) 2013 MatrixPilot. All rights reserved.
//

#if (WIN == 1 || NIX == 1)


#include "../../libUDB/libUDB.h"
#include "../../libUDB/heartbeat.h"
#include "SIL-filesystem.h"

#if 0

int FSInit(void)
{
	return 1;
}

FSFILE* FSfopen(const char* fileName, const char* mode)
{
//	printf("opening logfile %s\r\n", fileName);
	return fopen(fileName, mode);
}

int FSfclose(FSFILE* fo)
{
	return fclose(fo);
}

size_t FSfwrite(const void* data_to_write, size_t size, size_t n, FSFILE* stream)
{
	return fwrite(data_to_write, size, n, stream);
}

#else

#endif // 0

#endif // (WIN == 1 || NIX == 1)
