//
//  gettimeofday_win.h
//  MatrixPilot-SIL
//
//  Created by Ben Levitt on 3/2/13.
//  Copyright (c) 2013 MatrixPilot. All rights reserved.
//

#ifndef MatrixPilot_SIL_gettimeofday_win_h
#define MatrixPilot_SIL_gettimeofday_win_h

struct timezone
{
	int  tz_minuteswest; /* minutes W of Greenwich */
	int  tz_dsttime;     /* type of dst correction */
};

int gettimeofday(struct timeval *tv, struct timezone *tz);

#endif
