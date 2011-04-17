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


#undef xrateBUFF
#undef yrateBUFF
#undef zrateBUFF

#define	xrateBUFF	ADCBUF2
#define	yrateBUFF	ADCBUF3 
#define	zrateBUFF	ADCBUF1


#undef xaccelBUFF
#undef yaccelBUFF
#undef zaccelBUFF

#define	xaccelBUFF	ADCBUF4
#define	yaccelBUFF	ADCBUF5
#define	zaccelBUFF	ADCBUF6


#undef XRATE_SIGN
#undef YRATE_SIGN
#undef ZRATE_SIGN

#define XRATE_SIGN -
#define YRATE_SIGN -
#define ZRATE_SIGN -


#undef XACCEL_SIGN
#undef YACCEL_SIGN
#undef ZACCEL_SIGN

#define XACCEL_SIGN +
#define YACCEL_SIGN +
#define ZACCEL_SIGN +


#undef SCALEGYRO
#undef SCALEACCEL

#define SCALEGYRO 3.0
#define SCALEACCEL 2.64

#undef GPS_TYPE
#define GPS_TYPE GPS_UBX_4HZ
