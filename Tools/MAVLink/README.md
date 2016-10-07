# The setup of the mavlink and MAVproxy code beneath this directory

The mavlink code directory has been downloaded using git from:-
https://github.com/mavlink/mavlink
The commit ID of master used for MatrixPilot was 
e1668e31263353bf5e9a52de8a4084c5f25e35e0

There is a git submodule in the mavlink repository. 
This was also downloaded from:
https://github.com/ArduPilot/pymavlink
The commit ID of master used for MatrixPilot was
1641d7d4ff7e79b05d80821122972be66f5a9ec8 dated Thursday 6th October 2016.

The MAVproxy code was downloded from
https://github.com/tridge/MAVProxy
The commit ID of master used for MatrixPilot was
a170041cc1b1cf776ed727f09063f65c688bc99d dated Wednesday 5th October 2016.

The .git files for mavlink, pymavlink and MAVProxy were deleted, 
so that these files exist in their own right within 
the MatrixPilot respository. 

It is important to set the PYTHONPATH environment variable to to include
mavlink/pymavlink/generator before running the generation script below.
change working directory to
Tools/MAVLink/mavlink/pymavlink/generator
run "Python gen_MatrixPilot.py" to generate new C include files and a new python code for the MatrixPilot dialect.

I then had to delete the following line from options_mavlink.h
#define MAVLINK_EXTERNAL_RX_STATUS          1

After testing, and mavlink generation, the files were then commited 
to the master branch in th MatrixPilot git repository. 

Submodules were initially used during development, but these proved 
problematic, and so this simpler scheme has been adopted, 
as the mavlink generator is only updated once per year.

Peter Hollands 
October 2016

