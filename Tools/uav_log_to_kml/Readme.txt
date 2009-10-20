uav_log_to_kml.py is designed to be a generic UAV Telemetry Analyzer.
It reads output from a telemetry file - and turns it into KML that
allows a flight to be analyzed using Google Earth.

To see a picture of the possibilities - take a look at
http://picasaweb.google.co.uk/lh/photo/nfPkZ0y0-jWEBFdBulif_w?feat=directlink

As of early October 2009 this is early Alpha code. If you are not a developer, it may
not be suitable for you to use yet. It requires editing of the 
program source for you to use it, and it has no graphical user interface at this stage. 


GETTING STARTED:
Using aileronAssist_ben  you must first configure
options.h to send the correct format of telemetry out of the serial port.

In the options.h file, find the line that says :-
#define SERIAL_OUTPUT_FORMAT	SERIAL_NONE

and set it to :-
#define SERIAL_OUTPUT_FORMAT	SERIAL_UDB

Start up your plane and capture the telemetry into a file. (setting up 
wireless serial communications, or wired serial communications is beyond the scope
of this document).

For this example we will capture telemetry to flight15.TXT.
( In Windows I use Accessories / Communications / HyperTerminal,
In hyper terminal I do "Transfer / Capture Text" to a file called Flight15.Txt.
At the airfield, on my laptop,  I can see the telemetry coming into the terminal 
and know that it is all being recorded.

To convert your telemetry into KML files you will need to Install Python. The code is 
currently tested on Windows using this download:-

http://www.python.org/ftp/python/2.6.2/python-2.6.2.msi

If you don't have python yet - I strongly recommend using that version.
If you already have another fairly recent version (2.5x,2.6.x ) stay with that.

Run up the Python Developer Graphical User Interface (GUI) called IDLE.
Open the python file and change the following lines to reflect where your telemetry
is located. Here is an example from the python code in the file:

########################################################################
# CHANGE ME - for your computer. Where are you keeping your flight logs ?
# Please note the use of forward slashes is required on Windows OS
flight_log_dir = \
 'C:/Documents and Settings/petholla/Desktop/uav/flight_analysis/flight_logs/'
flight_log_name = 'flight15.TXT'
########################################################################

Those lines are near the bottom of the file.

In the GUI for Python press F5 or select "Run / Module".

The program will execute in about 2 seconds, and create two files.
For the example above, that would be flight15_pos.kmz (which shows the
flight path as vertical color coded translucent walls) and flight15_vec.kmz
which creates a model airplane showing pitch, roll and heading, for each telemetry
entry. (by the way, kmz files are simply zipped kml files).

 I do not recommend having more than 2000 such entries or Google Earth (GE)
may slow down and crawl.


If that is all set, then simply click on the kml files in Windows to launch Google Earth.

Load both kmz files. When GE starts, it's best to turn all the waypoints with a 
single click in the "Places" window. This prevents the screen from being to cluttered with
flight paths. (planes go around the course 3 times in the T3 competition). Then, turn on
the waypoints one at a time in "Places" window of GE.

This code has been shared early in its life. The wisdom of Open Source Community bulding is
"Release Early, Release Often". 

Please send feedback / support issues to the UAV Devboard discussion group at:-


FEATURES: 
Easy to use - just click on the resulting KML and view your flight in Google Earth.

Color codes flight paths in Manual Mode as Grey. Color Codes each flight path towards 
the next waypoint - making it easy to determine when the plane is making decisions.

Provides Pylon waypoint markers relative to the origin of booting up the plane - for easy 
analysis of the T3 competition course.

Plots miniature planes in GE for each line of telemetry - showing the plane's heading,
roll, and pitch. These placemarks, when clicked in the places window of GE, open up
 to show additional information at the moment, such as GPS Course over the Ground,
 and GPS Speed over Ground.

REQUIREMENTS:
You really just need Python and Google Earth.

PLATFORMS. In theory the program will work on Mac OS and Linux but not tested. 
Although the resulting KML files are tested to work on Ubuntu Linux.

UAV TELEMETRY SUPPORT:
Only works right now with the Branch called aileronAssist_ben -  and only if you switch on the serial output in options.h.

SUPPORT:
Please email your encouragement, questions,  constructive feedback
to the UAV Devboard mailing list at
http://groups.google.com/group/uavdevboard.

CURRENT TODOs:
1) Add Winzip support
2) Add "Pancakes" to mark altitude targets around waypoints.
3) Add Ardustation support (need some telemetry files to write and test code).
4) Agree standard protocols for UAV Devboard that match in options.h and serialIO.c
   (we need to settle on the main variables to catch).


Pete Hollands


