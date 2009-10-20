
uav_log_to_kml is designed to be a generic UAV Telemetry Analyser.
It reads output from a telemetry file - and turns it into KML that
allows a flight to be analysed using Google Earth.

To see a picture of the possiblities - take a look at
http://picasaweb.google.co.uk/lh/photo/nfPkZ0y0-jWEBFdBulif_w?feat=directlink

As of early October 2009 this is early Alpha code. If you are not a developer, it may
not be suitable for you to use yet. It requires editing of the program source for you to use it,
and it has no graphical user interface at this stage. 

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

GETTING STARTED: Want just see how it works ? Install Python. The code is currently tested
on Windows using this download to install python: 

http://www.python.org/ftp/python/2.6.2/python-2.6.2.msi

If you don't have python yet - I strongly recommend using that version.
If you already have another fairly recent version (2.5x,2.6.x ) stay with that.



Run up the Python Devloper Graphical User Interface (GUI) called IDLE.
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
For the example above, that would be flight15_pos.kml (which shows the
flight path as vertical color coded translucent walls) and flight15_vec.kml
which creates a model aeroplane showing pitch, roll and heading, for each telemetry
entry. I do not recommend having more than 2000 such entries or Google Earth (GE)
may slow down and crawl.

For the KML files to work, you must have a models directory where the KML has been created.
The models directory contains a small block plane from Google Sketckup that is inserted 
many time into flight15_vec.kml file. (block_plane.dae). The waypoints.dae file represents 
pylons in the sky around which the plane can race for the DIY Drones T3 Competition course.
The course is currently hard coded into code - but easy to comment out with a little reading.

If that is all set, then simply click on the kml files in Windows to launch Google Earth.

Load both KML files. When GE starts, it's best to turn all the waypoints with a 
single click in the "Places" window. This prevents the screen from being to cluttered with
flight paths. (planes go around the course 3 times in the T3 competition). Then, turn on
the waypoints one at a time in "Places" window of GE.

This code has been shared early in its life. The wisdom of Open Source Community bulding is
"Release Early, Release Often". 

Please send feedback / support issues to the UAV Devboard discussion group at:-


FEATURES: 

Easy to use - just click on the resulting KML and view your flight in Google Earth.

Colour codes flight paths in Manual Mode as Grey. Colour Codes each flight path towards the
next waypoint - making it easy to determine when the plane is making decisions.

Provides Pylon waypoint markers relative to the origin of booting up the plane - for easy 
analysis of the T3 competition course.

Creates miniature model planes in GE for each line of telemetry - showing the plane's heading,
roll, and pitch. These placemarks, when clicked in the places window of GE, open up to show
additional information at the moment, such as GPS Course over the Ground, and GPS Speed over Ground.

LIMITATIONS: The program does not yet produce a KMZ file - just a KML  file. 
That means, that you have to ensure that the models directory, containing models, is in the same directory as the KML files for them to
work with GE. That makes it hard to send your KML to someone else over the Internet - because
they will not have the models directory installed.

You can overcome this by manually creating KMZ files.
Using the example file above you would "add flight15_pos.kml" to flight15_pos.zip .
Then add all of the models directory (containing at least block_plane.dae and waypoints.dae)
to flight15_pos.zip. Then rename that file from flight15_pos.zip to flight15_pos.kmz . You now
have a file which can be shared with any worldwide to show your complete flight paths.
You have to do the same with flight15_vec.kml to create flight15_vec.kmz.

REQUIREMENTS: You really just need Python and Google Earth.

PLATFORMS. In theory the program will work on Mac OS and Linux but not tested. Although
the resulting KML files are tested to work on Ubuntu Linux.

UAV TELEMETRY SUPPORT: Only works right now with the Branch called aileronAssist_ben - 
and only if you switch on the serial output in options.h.

SUPPORT: Please email your encouragement, questions,  constructive feedback
to the UAV Devboard mailing list at
http://groups.google.com/group/uavdevboard.

CURRENT TODOs:
1) Add Winzip support
2) Add "Pancakes" to mark altitude targets around waypoings.
3) Add Ardustation support (need some telemetry files to write and test code).
4) Agree standard protocols for UAV Devboard that match in options.h and serioIO.c
   (we need to settle on the main variables to catch).


Pete Hollands


