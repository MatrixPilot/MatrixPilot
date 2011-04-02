ADDITIONAL INFORMATION REGARDING MAVLINK FOR MATRIXPILOT

This Mavlink directory has been created using the followign recipe:

* Downloaded and installed Windows GIT Tool from here
  http://code.google.com/p/msysgit/downloads/detail?name=Git-1.7.3.1-preview20101002.exe&can=2&q=

* Downloaded source code for Mavlink (XML data structures) from dev branch here:-
  https://github.com/pixhawk/mavlink

* Added in the file inttypes.h to the MAVLink include directory.

* Edited the QGroundControl website template to create matrixpilot_mavlink_bridge_header.h
  in the MAVLink include directory.

* Edited protocol.h to avoid compiler error that must be unique to dspic C30 compiler.
  Have to define integer used in for loop outside of the for loop. As  follows:-

  static inline void mavlink_send_uart(mavlink_channel_t chan, mavlink_message_t* msg)
  {
	// ARM7 MCU board implementation
	// Create pointer on message struct
	// Send STX
	comm_send_ch(chan, MAVLINK_STX);
	comm_send_ch(chan, msg->len);
	comm_send_ch(chan, msg->seq);
	comm_send_ch(chan, msg->sysid);
	comm_send_ch(chan, msg->compid);
	comm_send_ch(chan, msg->msgid);
	uint16_t i = 0;
	for(i = 0; i < msg->len; i++)
	{
		comm_send_ch(chan, msg->payload[i]);
	}
	comm_send_ch(chan, msg->ck_a);
	comm_send_ch(chan, msg->ck_b);
  }

* Added all the *.h files in MAVLink include directory and MAVLink/include/common directory
  to the MPLAB project list for MatrixPilot (relative file addressing). 
  (we are currently only using the "common" message types.


Pete Hollands 1st April 2011

  

