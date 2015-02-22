#ifndef _H_FLIGHTSTATE
#define _H_FLIGHTSTATE

/*
 */

#include "FlightState_sm.h"

struct FlightState
{
	/* If a string is acceptable, then this variable is set to YES;
	 * NO, otherwise.
	 */
	int isAcceptable;

	struct FlightStateContext _fsm;
};

extern void FlightState_Init(struct FlightState *this);
extern int FlightState_CheckString(struct FlightState *this, const char *theString);
extern void FlightState_Acceptable(struct FlightState *this);
extern void FlightState_Unacceptable(struct FlightState *this);

extern void FlightState_ent_Manual(struct FlightState *this);
extern void FlightState_ent_Stabilised(struct FlightState *this);
extern void FlightState_ent_Waypoint(struct FlightState *this);
extern void FlightState_ent_Return(struct FlightState *this);

extern void FlightState_ent_Calibrate(struct FlightState *this);
extern void FlightState_ent_Acquiring(struct FlightState *this);

#endif // _H_FLIGHTSTATE
