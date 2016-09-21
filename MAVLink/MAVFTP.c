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


#include "../MatrixPilot/defines.h"
#include "options_mavlink.h"

#if (USE_MAVLINK == 1)

#include "../MatrixPilot/MAVLink.h"
#include "MAVFTP.h"
#include <stdio.h>


boolean MAVFTPHandleMessage(mavlink_message_t* handle_msg)
{
	if (handle_msg->msgid == MAVLINK_MSG_ID_ENCAPSULATED_DATA)
	{
		mavlink_encapsulated_data_t packet;
		mavlink_msg_encapsulated_data_decode(handle_msg, &packet);
		return true;
	}
	return false;
}

void MAVFTPOutput_40hz(void)
{
}

#endif // (USE_MAVLINK == 1)
