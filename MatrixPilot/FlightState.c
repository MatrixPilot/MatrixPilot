/*
 */

#include "FlightState.h"

void FlightState_Init(struct FlightState *this)
{
	this->isAcceptable = 0;

	FlightStateContext_Init(&this->_fsm, this);

	/* Uncomment to see debug output. */
	/* setDebugFlag(&this->_fsm, 1); */
}

void FlightState_Acceptable(struct FlightState *this)
{
	this->isAcceptable = 1;
}

void FlightState_Unacceptable(struct FlightState *this)
{
	this->isAcceptable = 0;
}

int FlightState_CheckString(struct FlightState *this, const char *theString)
{
	while (*theString != '\0')
	{
		switch (*theString)
		{
		case '0':
//			FlightStateContext_Zero(&this->_fsm);
			break;

		case '1':
//			FlightStateContext_One(&this->_fsm);
			break;

		default:
//			FlightStateContext_Unknown(&this->_fsm);
			break;
		}
		++theString;
	}

	/* end of string has been reached - send the EOS transition. */
//	FlightStateContext_EOS(&this->_fsm);

	return this->isAcceptable;
}

void FlightState_ent_Manual(struct FlightState *this)
{
}

void FlightState_ent_Stabilised(struct FlightState *this)
{
}

void FlightState_ent_Waypoint(struct FlightState *this)
{
}

void FlightState_ent_Return(struct FlightState *this)
{
}

extern void FlightState_ent_Calibrate(struct FlightState *this)
{
}

extern void FlightState_ent_Acquiring(struct FlightState *this)
{
}

void test_FSM(struct FlightState *this)
{
	FlightStateContext_go_Manual(&this->_fsm);
	FlightStateContext_go_Stabilised(&this->_fsm);
	FlightStateContext_go_Waypoint(&this->_fsm);
	FlightStateContext_go_Return(&this->_fsm);
	FlightStateContext_go_Acquiring(&this->_fsm);
	FlightStateContext_go_Calibrate(&this->_fsm);

}

