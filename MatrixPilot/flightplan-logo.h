// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2012 MatrixPilot Team
// See the AUTHORS.TXT file for a list of authors of MatrixPilot.
//
// MatrixPilot is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MatrixPilot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MatrixPilot.  If not, see <http://www.gnu.org/licenses/>.


////////////////////////////////////////////////////////////////////////////////
// UDB LOGO Waypoint handling

// Move on to the next waypoint when getting within this distance of the current goal (in meters)
#define WAYPOINT_RADIUS 		25

// Origin Location
#define USE_FIXED_ORIGIN		0
#define FIXED_ORIGIN_LOCATION	{-1219950367, 374119664, 2.0}	// A point in Baylands Park in Sunnyvale, CA



////////////////////////////////////////////////////////////////////////////////
// Main Flight Plan

#define CIRCLE_RIGHT				1
#define CIRCLE_LEFT					2
#define INT_HANDLER_RIGHT			3
#define INT_HANDLER_LEFT			4

const struct logoInstructionDef instructions[] = {

IF_GT(REL_ANGLE_TO_HOME, 0)
	EXEC(CIRCLE_RIGHT)
ELSE
	EXEC(CIRCLE_LEFT)
END


TO (CIRCLE_RIGHT)
	USE_CURRENT_POS
	SET_INTERRUPT(INT_HANDLER_RIGHT)
	REPEAT_FOREVER
		FD(40)
		RT(10)
	END
END

TO (CIRCLE_LEFT)
	USE_CURRENT_POS
	SET_INTERRUPT(INT_HANDLER_LEFT)
	REPEAT_FOREVER
		FD(40)
		LT(10)
	END
END


TO (INT_HANDLER_RIGHT)
	IF_LT(RUDDER_INPUT_CHANNEL, 2800)
		EXEC(CIRCLE_LEFT)
	END
END

TO (INT_HANDLER_LEFT)
	IF_GT(RUDDER_INPUT_CHANNEL, 3200)
		EXEC(CIRCLE_RIGHT)
	END
END
};


/*
const struct logoInstructionDef instructions[] = {
	SET_INTERRUPT(7)
	
	DO_ARG(1, 10)
	END
	
	TO(1)
		IF_EQ(PARAM, 10)
			RT(1)
		ELSE
			RT(2)
		END
		
		IF_NE(PARAM, 10)
			IF_EQ(PARAM,1)
				REPEAT(2)
					RT(6)
				END
			END
			RT(3)
		ELSE
			IF_EQ(PARAM,1)
				REPEAT(2)
					RT(0)
				END
			ELSE
				REPEAT(2)
					RT(5)
				END
			END
			RT(4)
		END
		
		REPEAT(4)
			PARAM_ADD(1)
			RT_PARAM
		END
	END
	
	FD(99)
	
	TO(7)
		IF_LT(PARAM,80)
			EXEC(8)
		END
	END
	
	TO(8)
		RT(4)
		ELSE
		RT(2)
	END
} ;
*/


////////////////////////////////////////////////////////////////////////////////
// RTL Flight Plan
// 
// On entering RTL mode, turn off the engine, fly home, and circle indefinitely until touching down

const struct logoInstructionDef rtlInstructions[] = {
	
	// FLAG_ON(F_CROSS_TRACK)
	
	FLAG_ON(F_LAND)
	FD(50)
	
	FLAG_OFF(F_LAND)
	PEN_UP
	HOME
	USE_ANGLE_TO_GOAL
	BK(100)
	ALT_UP(100)
	PEN_DOWN
	
	FLAG_ON(F_LAND)
	USE_CURRENT_ANGLE
	
	REPEAT_FOREVER
		REPEAT(36)
			LT(10)
			FD(10)
		END
	END
};
