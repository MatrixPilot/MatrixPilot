// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009, 2010 MatrixPilot Team
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

// This file must be included after the board config files are defined


// Setup the defines for board orientation
#if (BOARD_ORIENTATION == ORIENTATION_FORWARDS)
	#define XAXIS_INVERTED 0
	#define YAXIS_INVERTED 0
	#define ZAXIS_INVERTED 0
#elif (BOARD_ORIENTATION == ORIENTATION_BACKWARDS)	// This orientation is still untested
	#define XAXIS_INVERTED 1
	#define YAXIS_INVERTED 1
	#define ZAXIS_INVERTED 0
#elif (BOARD_ORIENTATION == ORIENTATION_FLIPPED)	// This orientation is still untested
	#define XAXIS_INVERTED 0
	#define YAXIS_INVERTED 1
	#define ZAXIS_INVERTED 1
#elif (BOARD_ORIENTATION == ORIENTATION_INVERTED)
	#define XAXIS_INVERTED 1
	#define YAXIS_INVERTED 0
	#define ZAXIS_INVERTED 1
#endif


#if (XAXIS_INVERTED == 0)
	#if (XRATE_SIGN 1 == 1)
		#define XRATE_SIGN_ORIENTED +
	#else
		#define XRATE_SIGN_ORIENTED -
	#endif
	#if (XACCEL_SIGN 1 == 1)
		#define XACCEL_SIGN_ORIENTED +
	#else
		#define XACCEL_SIGN_ORIENTED -
	#endif
#else
	#if (XRATE_SIGN 1 == 1)
		#define XRATE_SIGN_ORIENTED -
	#else
		#define XRATE_SIGN_ORIENTED +
	#endif
	#if (XACCEL_SIGN 1 == 1)
		#define XACCEL_SIGN_ORIENTED -
	#else
		#define XACCEL_SIGN_ORIENTED +
	#endif
#endif


#if (YAXIS_INVERTED == 0)
	#if (YRATE_SIGN 1 == 1)
		#define YRATE_SIGN_ORIENTED +
	#else
		#define YRATE_SIGN_ORIENTED -
	#endif
	#if (YACCEL_SIGN 1 == 1)
		#define YACCEL_SIGN_ORIENTED +
	#else
		#define YACCEL_SIGN_ORIENTED -
	#endif
#else
	#if (YRATE_SIGN 1 == 1)
		#define YRATE_SIGN_ORIENTED -
	#else
		#define YRATE_SIGN_ORIENTED +
	#endif
	#if (YACCEL_SIGN 1 == 1)
		#define YACCEL_SIGN_ORIENTED -
	#else
		#define YACCEL_SIGN_ORIENTED +
	#endif
#endif


#if (ZAXIS_INVERTED == 0)
	#if (ZRATE_SIGN 1 == 1)
		#define ZRATE_SIGN_ORIENTED +
	#else
		#define ZRATE_SIGN_ORIENTED -
	#endif
	#if (ZACCEL_SIGN 1 == 1)
		#define ZACCEL_SIGN_ORIENTED +
	#else
		#define ZACCEL_SIGN_ORIENTED -
	#endif
#else
	#if (ZRATE_SIGN 1 == 1)
		#define ZRATE_SIGN_ORIENTED -
	#else
		#define ZRATE_SIGN_ORIENTED +
	#endif
	#if (ZACCEL_SIGN 1 == 1)
		#define ZACCEL_SIGN_ORIENTED -
	#else
		#define ZACCEL_SIGN_ORIENTED +
	#endif
#endif


// Define the gravity direction is the opposite of the Z accelerometer direction
// This is used for removing gravity from offsets
#if (ZACCEL_SIGN_ORIENTED 1 == -1)
	#define GRAVITY_SIGN +
#else
	#define GRAVITY_SIGN -
#endif
