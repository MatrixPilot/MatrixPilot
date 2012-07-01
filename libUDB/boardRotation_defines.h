// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2011 MatrixPilot Team
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

// ********** Note about sign conventions for anyone working on this file **************
// Acceleration axes are considered positive in the direction of gravity.
// Z_GRAVITY_SIGN and Y_GRAVITY_SIGN are opposite in sign to the orientation, they are used
// to remove gravity from the offset measurement
// **************************************************************************************


// Setup the defines for board orientation
#if (BOARD_ORIENTATION == ORIENTATION_FORWARDS)
	#define XAXIS_INVERTED 0
	#define YAXIS_INVERTED 0
	#define ZAXIS_INVERTED 0
#elif (BOARD_ORIENTATION == ORIENTATION_BACKWARDS)
	#define XAXIS_INVERTED 1
	#define YAXIS_INVERTED 1
	#define ZAXIS_INVERTED 0
#elif (BOARD_ORIENTATION == ORIENTATION_FLIPPED)
	#define XAXIS_INVERTED 0
	#define YAXIS_INVERTED 1
	#define ZAXIS_INVERTED 1
#elif (BOARD_ORIENTATION == ORIENTATION_INVERTED)
	#define XAXIS_INVERTED 1
	#define YAXIS_INVERTED 0
	#define ZAXIS_INVERTED 1
#elif (BOARD_ORIENTATION == ORIENTATION_ROLLCW)
	#define XAXIS_INVERTED 1
	#define YAXIS_INVERTED 0
	#define ZAXIS_INVERTED 0
#elif (BOARD_ORIENTATION == ORIENTATION_ROLLCW180)
	#define XAXIS_INVERTED 1
	#define YAXIS_INVERTED 1
	#define ZAXIS_INVERTED 1
#elif (BOARD_ORIENTATION == ORIENTATION_YAWCW)
	#define XAXIS_INVERTED 0
	#define YAXIS_INVERTED 1
	#define ZAXIS_INVERTED 0
#elif (BOARD_ORIENTATION == ORIENTATION_YAWCCW)
	#define XAXIS_INVERTED 1
	#define YAXIS_INVERTED 0
	#define ZAXIS_INVERTED 0
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

#if ((BOARD_ORIENTATION == ORIENTATION_FORWARDS)||(BOARD_ORIENTATION == ORIENTATION_BACKWARDS)||(BOARD_ORIENTATION == ORIENTATION_FLIPPED)||(BOARD_ORIENTATION == ORIENTATION_INVERTED))
//#define XRATE_VALUE	 ( XRATE_SIGN_ORIENTED ((udb_xrate.value) - (udb_xrate.offset) + vref_adj))
//#define YRATE_VALUE	 ( YRATE_SIGN_ORIENTED ((udb_yrate.value) - (udb_yrate.offset) + vref_adj))
//#define ZRATE_VALUE	 ( ZRATE_SIGN_ORIENTED ((udb_zrate.value) - (udb_zrate.offset) + vref_adj)) 
//#define XACCEL_VALUE ( XACCEL_SIGN_ORIENTED (( udb_xaccel.value ) - ( udb_xaccel.offset ) ))
//#define YACCEL_VALUE ( YACCEL_SIGN_ORIENTED (( udb_yaccel.value ) - ( udb_yaccel.offset ) ))
//#define ZACCEL_VALUE ( ZACCEL_SIGN_ORIENTED (( udb_zaccel.value ) - ( udb_zaccel.offset ) ))
#define XRATE_VALUE	 ( XRATE_SIGN_ORIENTED ((udb_xrate.value>>1) - (udb_xrate.offset>>1) + vref_adj))
#define YRATE_VALUE	 ( YRATE_SIGN_ORIENTED ((udb_yrate.value>>1) - (udb_yrate.offset>>1) + vref_adj))
#define ZRATE_VALUE	 ( ZRATE_SIGN_ORIENTED ((udb_zrate.value>>1) - (udb_zrate.offset>>1) + vref_adj)) 
#define XACCEL_VALUE ( XACCEL_SIGN_ORIENTED (( udb_xaccel.value>>1 ) - ( udb_xaccel.offset>>1 ) ))
#define YACCEL_VALUE ( YACCEL_SIGN_ORIENTED (( udb_yaccel.value>>1 ) - ( udb_yaccel.offset>>1 ) ))
#define ZACCEL_VALUE ( ZACCEL_SIGN_ORIENTED (( udb_zaccel.value>>1 ) - ( udb_zaccel.offset>>1 ) ))
#define UDB_XACCEL udb_xaccel
#define UDB_YACCEL udb_yaccel
#define UDB_ZACCEL udb_zaccel

// This determines the sign of correction used to remove gravity from the offset.
// It is opposite in sign to the sign of the alignment of the accelerometer axis.
// Used by horizontal initialization
#if (ZACCEL_SIGN_ORIENTED 1 == -1)
	#define Z_GRAVITY_SIGN +
#else
	#define Z_GRAVITY_SIGN -
#endif

// Used by vertical initialization
#if (YACCEL_SIGN_ORIENTED 1 == -1)
	#define Y_GRAVITY_SIGN +
#else
	#define Y_GRAVITY_SIGN -
#endif

#endif



#if ( (BOARD_ORIENTATION == ORIENTATION_ROLLCW) || (BOARD_ORIENTATION == ORIENTATION_ROLLCW180) )
#define ZRATE_VALUE	 ( XRATE_SIGN_ORIENTED ((udb_xrate.value>>1) - (udb_xrate.offset>>1) + vref_adj))
#define YRATE_VALUE	 ( YRATE_SIGN_ORIENTED ((udb_yrate.value>>1) - (udb_yrate.offset>>1) + vref_adj))
#define XRATE_VALUE	 ( ZRATE_SIGN_ORIENTED ((udb_zrate.value>>1) - (udb_zrate.offset>>1) + vref_adj)) 
#define ZACCEL_VALUE ( XACCEL_SIGN_ORIENTED (( udb_xaccel.value>>1 ) - ( udb_xaccel.offset>>1 ) ))
#define YACCEL_VALUE ( YACCEL_SIGN_ORIENTED (( udb_yaccel.value>>1 ) - ( udb_yaccel.offset>>1 ) ))
#define XACCEL_VALUE ( ZACCEL_SIGN_ORIENTED (( udb_zaccel.value>>1 ) - ( udb_zaccel.offset>>1 ) ))
#define UDB_ZACCEL udb_xaccel
#define UDB_YACCEL udb_yaccel
#define UDB_XACCEL udb_zaccel

// This determines the sign of correction used to remove gravity from the offset.
// It is opposite in sign to the sign of the alignment of the accelerometer axis.
// Used by horizontal initialization
#if (XACCEL_SIGN_ORIENTED 1 == -1)
	#define Z_GRAVITY_SIGN +
#else
	#define Z_GRAVITY_SIGN -
#endif

// Used by vertical initialization
#if (YACCEL_SIGN_ORIENTED 1 == -1)
	#define Y_GRAVITY_SIGN +
#else
	#define Y_GRAVITY_SIGN -
#endif

#endif



#if ( (BOARD_ORIENTATION == ORIENTATION_YAWCW) || (BOARD_ORIENTATION == ORIENTATION_YAWCCW) )
#define YRATE_VALUE	 ( XRATE_SIGN_ORIENTED ((udb_xrate.value>>1) - (udb_xrate.offset>>1) + vref_adj))
#define XRATE_VALUE	 ( YRATE_SIGN_ORIENTED ((udb_yrate.value>>1) - (udb_yrate.offset>>1) + vref_adj))
#define ZRATE_VALUE	 ( ZRATE_SIGN_ORIENTED ((udb_zrate.value>>1) - (udb_zrate.offset>>1) + vref_adj)) 
#define YACCEL_VALUE ( XACCEL_SIGN_ORIENTED (( udb_xaccel.value>>1 ) - ( udb_xaccel.offset>>1 ) ))
#define XACCEL_VALUE ( YACCEL_SIGN_ORIENTED (( udb_yaccel.value>>1 ) - ( udb_yaccel.offset>>1 ) ))
#define ZACCEL_VALUE ( ZACCEL_SIGN_ORIENTED (( udb_zaccel.value>>1 ) - ( udb_zaccel.offset>>1 ) ))
#define UDB_YACCEL udb_xaccel
#define UDB_XACCEL udb_yaccel
#define UDB_ZACCEL udb_zaccel

// This determines the sign of correction used to remove gravity from the offset.
// It is opposite in sign to the sign of the alignment of the accelerometer axis.
// Used by horizontal initialization
#if (ZACCEL_SIGN_ORIENTED 1 == -1)
	#define Z_GRAVITY_SIGN +
#else
	#define Z_GRAVITY_SIGN -
#endif

// Used by vertical initialization
#if (XACCEL_SIGN_ORIENTED 1 == -1)
	#define Y_GRAVITY_SIGN +
#else
	#define Y_GRAVITY_SIGN -
#endif

#endif
