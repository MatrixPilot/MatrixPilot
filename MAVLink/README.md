# MAVLINK FOR MATRIXPILOT

The following note about generating MAVLink header files in C and parsing code
in Python is for MatrixPilot developers. As these files are already generated,
users of MAVLink in MatrixPilot do not need to know how to generate MAVlink as
described below.

The Include directories for MAVLink are generated using Andrew Tridgell's
pymavlink libary and a utility call gen_MatrixPilot.py which can be found at
MatrixPilot/Tools/MAVLink/mavlink/pymavlink/generator/ .

The script reads the XML definitions that are "common" to all MAVLink protocol
enabled vehicles, as well as specific mesages for "matrixpilot". These two xml
files, common.xml and matrixpilot.xml can be found in -
Tools/MAVLink/mavlink/message_definitions/v1.0 .

Running gen_MatrixPilot.py will generate the C headers and copy them over to
main MatrixPilot MAVLink include directories. It will also generate a python
module for both creating and parsing the same messages. That will be in 
Tools/MAVLink/pymavlink/dialects/v10/matrixpilot.py 

MatrixPilot is using the MAVLink wire protocol 1.0. 
Although the generator supports wire protocol 2.0 for encryption, at the time of writing, this has not yet seen active development and use in MatrixPilot.

gen_MatrixPilot.py currently has to be run from within a DOS window in Windows.

Pete Hollands
October 2016
