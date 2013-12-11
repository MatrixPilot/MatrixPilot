
ADDITIONAL INFORMATION REGARDING MAVLINK FOR MATRIXPILOT

October 2012

The following note about generating MAVLink header files in C and parsing code in
Python is for developers of MatrixPilot. As these files are already generated,
users of MAVLink in MatrixPilot do not need to know how to generate MAVlink as described below.

The Include directories for MAVLink are now generated using Andrew Tridgell's
pymavlink libary and a utility call gen_matrixpilot.py . We no longer are using QGroundControl to generate header files.
You will find the relevant scripts in
the tools directory. e.g. Tools/MAVLink/pymavlink/generator/gen_matrixpilot.py .

The script reads the XML definitions that are "common" to all MAVLink protocol enabled vehicles,
as well as specific mesages for "matrixpilot". These two xml files, common.xml and matrixpilot.xml can be found in
Toools/MAVlink/pymavlink/generator/message-definitions .

You will need to have Python installed. The scripts have been tested under Windows7 64 bit,
with Python 2.7.1 and Python 2.7.2.  They have also run correctly under Ubuntu 10.4 with Python 2.6.

Running gen_matrixpilot.py will generate the C headers and copy them over to main MatrixPilot
MAVLink include directories. It will also generate a python module for both creating and parsing the 
same messages. That will be in Tools/MAVLink/pymavlink/generator/python/mavlink_matrixpilot_v0.9.py . 

We are using the MAVLink wire protocol 1.0. 
Mavlink wire protocol 0.9 is not supported.

gen_matrixpilot.py currently has to be run from within a DOS window in Windows.

Pete Hollands


=========================================================================
By comparison, here is the original reciped that was used to create 
the MAVLink include directory. This is now deprecated:-


The Mavlink directory can also be created using the followign recipe:

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

Pete Hollands


  

